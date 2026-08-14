#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

#define DELIMITERS " \t\r\n\a"

int shell_execute(std::vector<std::string> &args) // TODO: Use windows CreateProcess and CreateThread to implement process forking + exec.
{
    return EXIT_SUCCESS;
}

void shell_parse(const std::string &line, std::vector<std::string> &args)
{
    std::stringstream line_stream(line);
    std::string arg;
    while(std::getline(line_stream, arg, DELIMITERS[0])) { // TODO: Add support for quoted arguments, and other delimiters later
        if(!arg.empty()) {
            args.push_back(arg);
        }
    }
}

void shell_loop(void)
{
    std::string line;
    std::vector<std::string> args;
    int status;

    do
    {
        std::cout << "> ";
        std::getline(std::cin, line); // TODO: Add support for multi-line and file commands later, shell_read_line() function will be created for that
        shell_parse(line, args);
        status = shell_execute(args);

        line.clear();
        args.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (!status);
}

int main()
{

    shell_loop();

    return EXIT_SUCCESS;
}