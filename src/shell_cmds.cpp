#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <system_error>
#include "shell_cmds.hpp"
#include "utils.hpp"

std::map<std::string, int(*)(std::vector<std::string> &, std::ostream*, std::ostream*, ShellContext &)> shell_cmds = {
    {"cd", shell_chdir},
    {"complete", shell_complete},
    {"echo", shell_echo},
    {"exit", shell_exit},
    {"history", shell_history},
    {"jobs", shell_jobs},
    {"list", shell_listdir},
    {"type", shell_type},
    {"pwd", shell_pwd},
    {"help", shell_help}
};

std::set<std::string> whiteList = {
    "echo",
    "help",
    "type"
};

void shell_join_builtin_jobs(ShellContext& context)
{
    for (auto& [id, job] : context.jobs->builtinJobs) {
        if (job->worker.joinable()) {
            job->worker.join();
        }
    }

    context.jobs->builtinJobs.clear();
}

// TODO: build handling for paths with spaces
int shell_chdir(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    std::string new_dir(args[1]);

    try {
        const char* home = nullptr;
        if(args[1][0] == '~') {
            home = std::getenv("USERPROFILE");
            std::string currPath(new_dir);
            currPath.erase(0, 1);
            std::string finalPath = home + currPath;
            fs::path dir(finalPath);
            populate_dir(context.CurrDir, dir);
            context.newDir->clear();
            context.newDir = new Trie(*context.CurrDir);
            fs::current_path(dir);
        } else {
            fs::path dir(new_dir);
            populate_dir(context.CurrDir, dir);
            context.newDir->clear();
            context.newDir = new Trie(*context.CurrDir);
            fs::current_path(dir);
        }
    } catch (const fs::filesystem_error e) {
        *err << "cd: " << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}

int shell_complete(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    try {
        if(args[1] == "-p") {
            if(args.size() != 3) {
                throw std::invalid_argument("Invalid arguments");
            }
            std::string cmd = args[2];
            if(context.cmdComp.find(cmd) != context.cmdComp.end()) {
                for(auto it : context.cmdComp[cmd]->currCompSet) {
                    *out << it << " ";
                }
                *out << std::endl;
            } else {
                *out << "complete: " << cmd << ": no completion specification" << std::endl;
            }
        } else if (args[1] == "-C") {
            if(args.size() != 4) {
                throw std::invalid_argument("Invalid arguments");
            }
            std::string comp_path = args[2];
            std::string cmd = args[3];
            context.cmdComp[cmd] = new CompleteInfo();
            for(auto it : args) {
                context.cmdComp[cmd]->currCompSet.push_back(it);
            }
            context.cmdComp[cmd]->execPath = comp_path;
            context.cmdComp[cmd]->compFlag = COMPLETE_AUTOCOMPLETE_SCRIPT;
        } else if (args[1] == "-r") {
            if(args.size() != 3) {
                throw std::invalid_argument("Invalid arguments");
            }
            std::string del = args[2];
            auto it = context.cmdComp.find(del);
            if(it != context.cmdComp.end()) {
                context.cmdComp.erase(it);
            }
        } else if (args[1] == "-f") {
            if(args.size() != 3) {
                throw std::invalid_argument("Invalid arguments");
            }
            std::string cmd = args[2];
            context.cmdComp[cmd] = new CompleteInfo();
            for(auto it : args) {
                context.cmdComp[cmd]->currCompSet.push_back(it);
            }
            context.cmdComp[cmd]->compFlag = COMPLETE_AUTOCOMPLETE_FILE;    
        } else if (args[1] == "-d") {
            if(args.size() != 3) {
                throw std::invalid_argument("Invalid arguments");
            }
            std::string cmd = args[2];
            context.cmdComp[cmd] = new CompleteInfo();
            for(auto it : args) {
                context.cmdComp[cmd]->currCompSet.push_back(it);
            }
            context.cmdComp[cmd]->compFlag = COMPLETE_AUTOCOMPLETE_DIRECTORY;    
        }

    } catch (const std::invalid_argument e) {
        *err << "complete: " << e.what() << std::endl;
    }
exit:
    return EXIT_SUCCESS;
}

int shell_echo(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    for(int i = 1; i < args.size(); i++) {
        *out << args[i] << " ";
    }
    *out << std::endl;
    return EXIT_SUCCESS;
}

int shell_exit(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    *out << "Exiting WinShell..." << std::endl;
    shell_join_builtin_jobs(context);
    exit(EXIT_SUCCESS);
}

int shell_help(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    *out << "WinShell Help: " << std::endl;
    *out << "Write program names and arguments, and hit enter" << std::endl;
    *out << "The following commands are available by default:" << std::endl;
    for(auto it : shell_cmds) {
        *out << "  " << it.first << std::endl;
    }
    // TODO: Add man command print here once implemented
    return EXIT_SUCCESS;
}

int shell_history(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    std::size_t ind = 1;
    std::size_t st = 1;
    if(args.size() > 1 && args[1] == "-r") {
        if(args.size() > 2 && fs::exists(args[2])) {
            std::ifstream file(args[2]);
            std::string line;
            while(getline(file, line)) {
                context.history.push_back(line);
            }
        } else {
            *err << "Enter valid file path\n" << std::flush;
            return EXIT_SUCCESS;
        }
    } else if (args.size() > 1 && args[1] == "-w") {
        if(args.size() > 2) {
            std::ofstream file;
            file.open(args[2]);
            for(auto it : context.history) {
                file << it << std::endl;
            }
        } else {
            *err << "Unable to load file\n" << std::flush;
            return EXIT_SUCCESS;
        }
    } else if (args.size() > 1 && args[1] == "-a") {
        if(args.size() > 2) {
            std::ofstream file(args[2], std::ios::app);
            for(auto it : context.history) {
                file << it << std::endl;
            }
        } else {
            *err << "Unable to load file\n" << std::flush;
            return EXIT_SUCCESS;
        }
    } else if(args.size() > 1) {
        try {
            st = context.history.size() - static_cast<std::size_t>(std::stoull(args[1])) + 1;
        } catch (...) {
            throw;
        }
    }
    if(args.size() == 1 || (args.size() > 1 && isNum(args[1]))) {
        for(auto it : context.history) {
            if(ind >= st) {
                *out << ' ';
                *out << ' ';
                *out << ind;
                *out << ' ';
                *out << ' ';
                *out << it;
                *out << '\n';
            }
            ind++;
        }
    }

    return EXIT_SUCCESS;
}

int shell_jobs(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    std::map<int, ProcessInfo> *jobs = &context.jobs->processJobs;

    for(auto it : *jobs) {
        DWORD pid = it.second.pid;
        ProcessStatus status = procStatus(pid);
        if(status == ProcessStatus::Running) {
            *out << '[' << it.first << ']';
            *out << '+';
            *out << ' ';
            *out << it.second.status;
            *out << "\t\t";
            *out << it.second.cmd;
            *out << '\n';
        } else {
            jobs->erase(it.first);
            context.jobs->processCount--;
        }
    }

    auto builtinJobs = &context.jobs->builtinJobs;
    for(auto it = builtinJobs->begin(); it != builtinJobs->end(); ++it) {
        BuiltinJob& job = *it->second;
        bool done = job.done.load(std::memory_order_acquire);

        if(done) {
            if(job.worker.joinable()) {
                job.worker.join();
            }
            builtinJobs->erase(it);
        } else {
            *out << "[B" << it->first << "] "
            << (done ? "Done" : "Running")
            << "\t" << job.cmd << '\n';
        }
    }

    return EXIT_SUCCESS;
}

int shell_listdir(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    fs::path curr_dir = fs::current_path();
    if(args.size() > 1) {
        if(args[1][0] == '~') {
            const char* home = std::getenv("USERPROFILE");
            fs::path homedir(home);
            std::string rel_dir(args[1].size() > 2 ? args[1].begin() + 2 : args[1].begin() + 1, args[1].end());
            fs::path reldir(rel_dir);
            curr_dir = homedir / reldir;
        } else {
            fs::path nwdir(args[1]);
            curr_dir = curr_dir / nwdir;
        }
    }
    if (fs::is_directory(curr_dir)) {

        std::vector<std::string> entries;

        for (const auto& entry : fs::directory_iterator(curr_dir)) {
            if (fs::is_directory(entry.path())) {
                entries.push_back(entry.path().filename().string() + "\\");
            }
            else if (fs::is_regular_file(entry.path())) {
                entries.push_back(entry.path().filename().string());
            }
        }

        const int columnWidth = 30;

        for (size_t i = 0; i < entries.size(); ++i) {
            *out << std::left << std::setw(columnWidth) << entries[i];

            if ((i + 1) % 3 == 0)
                *out << '\n';
        }

        // Newline if the last row wasn't complete
        if (entries.size() % 3 != 0)
            *out << '\n';
    }
    return EXIT_SUCCESS;
}

int shell_pwd(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    fs::path curr_dir = fs::current_path();
    std::string currPath = curr_dir.string();
    *out << currPath << std::endl;
    return EXIT_SUCCESS;
}

int shell_type(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    int cmdType = SHELL_CMD_TYPE_UNKNOWN;
    bool isCommand = context.path->search(args[1]);
    std::string directory, finaldir;
    if(isCommand) {
        std::vector<std::string> exec = context.path->getInfo(args[1]);
        if(exec[0] == "shell-command") {
            cmdType = SHELL_CMD_TYPE_BUILTIN;
        } else {
            cmdType = SHELL_CMD_TYPE_EXEC;
            directory = exec[0];
            std::string exe;
            for(int i = directory.size() - 1; i >= 0; i--) {
                if(directory[i] == '/' || directory[i] == '\\') break;
                exe += directory[i];
            }
            reverse(exe.begin(), exe.end());
            finaldir = remove_end(directory, exe);
        }
    }

    switch(cmdType) {
        case SHELL_CMD_TYPE_BUILTIN:
            *out << args[1] << " is a shell builtin." << std::endl;
            break;
        case SHELL_CMD_TYPE_EXEC:
            *out << args[1] << " is " << finaldir << std::endl;
            break;
        default:
            *out << "type: " << args[1] <<": not found" << std::endl;
    }
    return EXIT_SUCCESS;
}

int shell_cmd_handler(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    if (args.empty()) return EXIT_FAILURE;

    if(args[args.size() - 1] == "&" && whiteList.find(args[0]) != whiteList.end()) {
        auto job = std::make_unique<BuiltinJob>();
        int jobId;
        BuiltinJob* jobPtr;
        const auto builtin = shell_cmds[args[0]];
        std::vector<std::string> workerArgs;
        // remove &
        args.pop_back();
        workerArgs = args;
        
        for (std::size_t i = 0; i < args.size(); ++i) {
            if (i != 0) job->cmd += ' ';
            job->cmd += args[i];
        }

        jobPtr = job.get();
        context.jobs->builtinJobs.emplace(jobId, std::move(job));
        try {
            jobPtr->worker = std::thread(
                [
                    jobPtr,
                    builtin,
                    workerArgs,
                    out,
                    err,
                    contextPtr = &context
                ] () mutable {
                    try {
                        static std::mutex outputMutex;
                        std::lock_guard<std::mutex> lock(outputMutex);
                        builtin(workerArgs, out, err, *contextPtr);
                    } catch (...) {
                        jobPtr->exitCode = EXIT_FAILURE;
                        throw;
                    }
                    jobPtr->done.store(true, std::memory_order_release);
                }
            );
        } catch (const std::system_error& e) {
            *err << "shell: failed to start background job: " << e.what() << '\n';
            return EXIT_FAILURE;
        }
    } else {
        // remove & if present
        if(args[args.size() - 1] == "&") args.pop_back();
        if(!(shell_cmds.find(args[0]) == shell_cmds.end()))
            return shell_cmds[args[0]](args, out, err, context);
    }

    return EXIT_FAILURE;
}
