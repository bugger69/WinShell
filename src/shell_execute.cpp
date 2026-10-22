#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include "shell_context.hpp"
#include "parser.hpp"
#include "shell_execute.hpp"
#include "shell_cmds.hpp"
#include "utils.hpp"
#include "pipe_stream.hpp"

namespace {
    class ScopedHandle {
    private:
        HANDLE handle = INVALID_HANDLE_VALUE;

    public:
        ScopedHandle() = default;
        explicit ScopedHandle(HANDLE value) : handle(value) {}
        ~ScopedHandle() {
            if(handle != INVALID_HANDLE_VALUE && handle != nullptr) {
                CloseHandle(handle);
            }
        }

        ScopedHandle(const ScopedHandle&) = delete;
        ScopedHandle& operator=(const ScopedHandle&) = delete;

        ScopedHandle(ScopedHandle&& other) noexcept : handle(other.release()) {}
        ScopedHandle& operator=(ScopedHandle&& other) noexcept {
            if(this != &other) {
                reset(other.release());
            }
            return *this;
        }

        HANDLE get() const noexcept {
            return handle;
        }

        HANDLE release() noexcept {
            HANDLE value = handle;
            handle = INVALID_HANDLE_VALUE;
            return value;
        }

        void reset(HANDLE value = INVALID_HANDLE_VALUE) noexcept {
            if(handle != INVALID_HANDLE_VALUE && handle != nullptr) {
                CloseHandle(handle);
            }
            handle = value;
        }

        explicit operator bool() const noexcept {
            return handle != INVALID_HANDLE_VALUE && handle != nullptr;
        }
    };

    ScopedHandle duplicate_inheritable_handle(HANDLE source) {
        HANDLE duplicate = INVALID_HANDLE_VALUE;
        if(source == nullptr || source == INVALID_HANDLE_VALUE ||
           !DuplicateHandle(GetCurrentProcess(), source, GetCurrentProcess(),
                           &duplicate, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
            throw std::runtime_error("Failed to duplicate a pipe handle");
        }
        return ScopedHandle(duplicate);
    }

    void create_pipe(ScopedHandle& readHandle, ScopedHandle& writeHandle) {
        HANDLE read = INVALID_HANDLE_VALUE;
        HANDLE write = INVALID_HANDLE_VALUE;
        if(!CreatePipe(&read, &write, nullptr, 0)) {
            throw std::runtime_error("CreatePipe failed with error " +
                                     std::to_string(GetLastError()));
        }
        readHandle.reset(read);
        writeHandle.reset(write);
    }

    void pump_pipe_to_stream(HANDLE ownedReadHandle, std::ostream* output) {
        ScopedHandle readHandle(ownedReadHandle);
        char buffer[4096];
        DWORD bytesRead = 0;
        bool outputFailed = false;
        while(ReadFile(readHandle.get(), buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0) {
            if(!outputFailed) {
                try {
                    output->write(buffer, static_cast<std::streamsize>(bytesRead));
                    outputFailed = !*output;
                } catch(...) {
                    outputFailed = true;
                }
            }
        }
        if(!outputFailed) {
            try {
                output->flush();
                outputFailed = !*output;
            } catch(...) {
                outputFailed = true;
            }
        }
        if(outputFailed) {
            std::cerr << "Failed to write pipeline output" << std::endl;
        }
    }

    void drain_pipe(HANDLE ownedReadHandle) {
        ScopedHandle readHandle(ownedReadHandle);
        char buffer[4096];
        DWORD bytesRead = 0;
        while(ReadFile(readHandle.get(), buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0) {
        }
    }

    PROCESS_INFORMATION launch_pipeline_process(
        std::vector<std::string>& args,
        ShellContext& context,
        HANDLE input,
        HANDLE output,
        HANDLE error
    ) {
        int found = EXIT_FAILURE;
        const std::string execPath = shell_find_exec(args, found, context);
        if(found == EXIT_FAILURE) {
            throw std::runtime_error("Command not found: " + args[0]);
        }

        std::wstring command;
        for(std::size_t i = 0; i < args.size(); ++i) {
            const std::string& arg = args[i];
            const std::string& text = i == 0 ? execPath : arg;
            command += std::wstring(text.begin(), text.end());
            command += L" ";
        }

        ScopedHandle childInput = duplicate_inheritable_handle(input);
        ScopedHandle childOutput = duplicate_inheritable_handle(output);
        ScopedHandle childError = duplicate_inheritable_handle(error);

        STARTUPINFOW startup{};
        startup.cb = sizeof(startup);
        startup.dwFlags = STARTF_USESTDHANDLES;
        startup.hStdInput = childInput.get();
        startup.hStdOutput = childOutput.get();
        startup.hStdError = childError.get();

        PROCESS_INFORMATION process{};
        if(!CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE, 0,
                           nullptr, nullptr, &startup, &process)) {
            throw std::runtime_error("CreateProcessW failed with error " +
                                     std::to_string(GetLastError()));
        }
        return process;
    }
}

std::string shell_find_exec(std::vector<std::string> &args, int &found, ShellContext &context) { // TODO: Add support for full file paths and executables other than .exe(s)
    std::string suff = ".exe";
    std::string exe = remove_end(args[0], suff);
    std::string exec_path;
    bool isFound = context.path->search(exe);
    if(isFound) {
        found = EXIT_SUCCESS;
        std::vector<std::string> execs = context.path->getInfo(exe);
        exec_path = execs[0]; // TODO: Fig out something for multiple execs
    }

    return exec_path;
}

int shell_process_launch(std::vector<std::string> &args, STARTUPINFOW &si, PROCESS_INFORMATION &pi, std::ostream* out, std::ostream* err, ShellContext &context)
{
    int cmd_found = EXIT_FAILURE;
    bool execInBackground = false;
    std::string cmdstat = "Not Running";
    std::wstring command;
    std::string execPath = shell_find_exec(args, cmd_found, context);
    SECURITY_ATTRIBUTES sa{}; // create security attributes to enable inheritance
    OutputTarget *consoleOut = context.consoleOut;
    HANDLE outputHandle = INVALID_HANDLE_VALUE;
    HANDLE errHandle = INVALID_HANDLE_VALUE;
    int n = args[args.size() - 1] == "&" ? args.size() - 1 : args.size();

    if(args[args.size() - 1] == "&") {
        execInBackground = true;
    }

    if (out != &std::cout) {  // check if filestream is opn, closes it in that case
        if (auto* fileStream = dynamic_cast<std::ofstream*>(out)) {
            fileStream->close();
        }
    }

    if(err != &std::cerr) {
        if (auto* fileStream = dynamic_cast<std::ofstream*>(err)) {
            fileStream->close();
        }
    }

    if(cmd_found == EXIT_FAILURE) return EXIT_FAILURE;

    if (consoleOut->flag_std == SHELL_CMD_OUT_STD_FILE || consoleOut->flag_std == SHELL_CMD_OUT_STD_APPEND) {
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;
        
        std::wstring filename( // create windows string
            consoleOut->file_std.begin(),
            consoleOut->file_std.end()
        );
        // TODO: Ensure this doesn't get called if there's a pipe
        outputHandle = CreateFileW( // create file Write Handler
            filename.c_str(),
            consoleOut->flag_std == SHELL_CMD_OUT_STD_FILE ? GENERIC_WRITE : FILE_APPEND_DATA,
            FILE_SHARE_WRITE, // gotta figure out how this works
            &sa,
            consoleOut->flag_std == SHELL_CMD_OUT_STD_FILE ? CREATE_ALWAYS : OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (outputHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create output file\n" << std::endl;
            return EXIT_SUCCESS;
        } else {
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdOutput = outputHandle; // add pipe conditions here too
        }
    }

    if (consoleOut->flag_err == SHELL_CMD_OUT_ERR_FILE || consoleOut->flag_err == SHELL_CMD_OUT_ERR_APPEND) {
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;
        
        std::wstring filename( // create windows string
            consoleOut->file_err.begin(),
            consoleOut->file_err.end()
        );

        errHandle = CreateFileW( // create file Write Handler
            filename.c_str(),
            consoleOut->flag_err == SHELL_CMD_OUT_ERR_FILE ? GENERIC_WRITE : FILE_APPEND_DATA,
            FILE_SHARE_WRITE, // gotta figure out how this works
            &sa,
            consoleOut->flag_err == SHELL_CMD_OUT_ERR_FILE ? CREATE_ALWAYS : OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (errHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create error file\n" << std::endl;
            return EXIT_SUCCESS;
        } else {
            si.dwFlags |= STARTF_USESTDHANDLES;
            // si.hStdOutput = outputHandle;
            si.hStdError = errHandle;
        }
    }

    for (int i = 0; i < n; i++) {
        std::string arg = args[i];
        if(i == 0) {
            command += std::wstring(execPath.begin(), execPath.end()) + L" ";
        } else {
            command += std::wstring(arg.begin(), arg.end()) + L" ";
        }
    }
    
    bool status = CreateProcessW(
        NULL,                   // No module name (use command line)
        command.data(),         // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        TRUE,                  // Set handle inheritance to TRUE
        0,                      // No creation flags
        NULL,                   // Use parent's environment block
        NULL,                   // Use parent's starting directory 
        &si,                    // Pointer to STARTUPINFO structure
        &pi                     // Pointer to PROCESS_INFORMATION structure
    );

    if(status && !execInBackground) {
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exit_code;
        if (GetExitCodeProcess(pi.hProcess, &exit_code)) {
            std::cout << "Process exited with code: " << exit_code << std::endl;
        } else {
            std::cerr << "Error: GetExitCodeProcess failed with error code " << GetLastError() << std::endl;
        }
        CloseHandle(outputHandle); // closing output handle 
        CloseHandle(errHandle); // closing error handle
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else if (status && execInBackground) {
        ProcessInfo *proc;
        int pid = pi.dwProcessId;
        std::string cmdstr(command.begin(), command.end());
        cmdstat = "Running";
        context.jobs->processCount += 1;

        for(int i = 0; i <= context.jobs->processCount; i++) {
            if(context.jobs->processJobs.find(i) == context.jobs->processJobs.end()) {
                proc = &context.jobs->processJobs[i];
            }
        }

        proc->pid = pid;
        proc->cmd = cmdstr;
        proc->status = cmdstat;

        std::cout << '[' << 0 << ']' << ' ' << pid << '\n';

        CloseHandle(outputHandle); // closing output handle 
        CloseHandle(errHandle); // closing error handle
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "Error: CreateProcess failed with error code " << GetLastError() << std::endl;
    }

    return EXIT_SUCCESS;
}

int shell_execute(std::vector<std::string> &args, ShellContext &context) // TODO: add basic commands
{
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    int status;
    OutputTarget *consoleOut = context.consoleOut;
    std::ofstream file, errFile;
    std::ostream* out = &std::cout;
    std::ostream* err = &std::cerr;
    std::vector<std::vector<std::string>> cmdVec;

    if(args.empty()) goto end;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if(consoleOut->flag_std == SHELL_CMD_OUT_STD_FILE || consoleOut->flag_std == SHELL_CMD_OUT_STD_APPEND) {
        if(consoleOut->file_std == "") goto end;
        consoleOut->flag_std == SHELL_CMD_OUT_STD_FILE ? file.open(consoleOut->file_std) : file.open(consoleOut->file_std, std::ios::app);
        if (!file) {
            std::cerr << "Failed to open output file" << std::endl;
            goto end;
        }
        out = &file;
    }

    if(consoleOut->flag_err == SHELL_CMD_OUT_ERR_FILE || consoleOut->flag_err == SHELL_CMD_OUT_ERR_APPEND) {
        if(consoleOut->file_err == "") goto end;
        consoleOut->flag_err == SHELL_CMD_OUT_ERR_FILE ? errFile.open(consoleOut->file_err) : errFile.open(consoleOut->file_err, std::ios::app);
        if (!errFile) {
            std::cerr << "Failed to open output file" << std::endl;
            goto end;
        }
        err = &errFile;
    }

    cmdVec = parse_for_symbol(args, ";");
    for(auto it : cmdVec) {
        int status = EXIT_FAILURE, dum = 0;
        bool pipelineErrorReported = false;
        std::vector<std::vector<std::string>> pipedCmds;

        if(!it.size()) continue;

        pipedCmds = parse_for_symbol(it, "|");
        if(pipedCmds.size() <= 1) {
            if(shell_find_exec(it, dum, context) == "shell-command") {
                status = shell_cmd_handler(it, out, err, context);
            } else {
                status = shell_process_launch(it, si, pi, out, err, context);
            }
        } else {
            std::vector<PROCESS_INFORMATION> processes;
            std::vector<std::thread> workers;
            ScopedHandle previousRead;
            ScopedHandle errorRead;
            ScopedHandle errorWrite;

            try {
                create_pipe(errorRead, errorWrite);
                workers.emplace_back(pump_pipe_to_stream, errorRead.release(), err);

                for(std::size_t i = 0; i < pipedCmds.size(); ++i) {
                    std::vector<std::string>& cmds = pipedCmds[i];
                    if(cmds.empty()) {
                        throw std::runtime_error("Invalid empty command in pipeline");
                    }

                    const bool isLast = i + 1 == pipedCmds.size();
                    int commandFound = EXIT_FAILURE;
                    const bool isBuiltin =
                        shell_find_exec(cmds, commandFound, context) == "shell-command";

                    if(isBuiltin) {
                        if(previousRead) {
                            workers.emplace_back(drain_pipe, previousRead.release());
                        }

                        if(isLast) {
                            status = shell_cmd_handler(cmds, out, err, context);
                        } else {
                            ScopedHandle nextRead;
                            ScopedHandle nextWrite;
                            create_pipe(nextRead, nextWrite);
                            workers.emplace_back(
                                [cmds, writeHandle = nextWrite.release(), err, &context]() mutable {
                                    PipeOutStream pipeOutput(writeHandle);
                                    try {
                                        shell_cmd_handler(cmds, &pipeOutput, err, context);
                                    } catch(const std::exception& e) {
                                        *err << "Pipeline builtin failed: " << e.what() << '\n';
                                    } catch(...) {
                                        *err << "Pipeline builtin failed with an unknown error\n";
                                    }
                                }
                            );
                            previousRead = std::move(nextRead);
                            status = EXIT_SUCCESS;
                        }
                    } else {
                        ScopedHandle nextRead;
                        ScopedHandle nextWrite;
                        ScopedHandle finalRead;
                        ScopedHandle finalWrite;

                        if(isLast) {
                            create_pipe(finalRead, finalWrite);
                            workers.emplace_back(pump_pipe_to_stream, finalRead.release(), out);
                        } else {
                            create_pipe(nextRead, nextWrite);
                        }

                        HANDLE input = previousRead
                            ? previousRead.get()
                            : GetStdHandle(STD_INPUT_HANDLE);
                        HANDLE output = isLast ? finalWrite.get() : nextWrite.get();
                        PROCESS_INFORMATION process = launch_pipeline_process(
                            cmds, context, input, output, errorWrite.get()
                        );
                        processes.push_back(process);

                        previousRead.reset();
                        if(!isLast) {
                            previousRead = std::move(nextRead);
                        }
                        status = EXIT_SUCCESS;
                    }
                }
            } catch(const std::exception& e) {
                *err << "Pipeline failed: " << e.what() << std::endl;
                status = EXIT_FAILURE;
                pipelineErrorReported = true;
            }

            previousRead.reset();
            errorWrite.reset();
            for(auto& process : processes) {
                WaitForSingleObject(process.hProcess, INFINITE);
                CloseHandle(process.hProcess);
                CloseHandle(process.hThread);
            }
            for(auto& worker : workers) {
                if(worker.joinable()) {
                    worker.join();
                }
            }
        }

        if(status == EXIT_FAILURE && !pipelineErrorReported) {
            std::cerr << args[0] << ": command not found" << std::endl;
        }
    }

end: // TODO: Reset consoleout here
    consoleOut->flag_std = SHELL_CMD_OUT_STD_DEF;
    consoleOut->file_std = "";
    consoleOut->flag_err = SHELL_CMD_OUT_ERR_DEF;
    consoleOut->file_err = "";
    return EXIT_SUCCESS;
}