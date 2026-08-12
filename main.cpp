#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

int shell_execute(std::vector<std::string> &args)
{
    return EXIT_SUCCESS;
}

void shell_parse(const std::string &line, std::vector<std::string> &args)
{
    std::stringstream line_stream(line);
    std::string arg;
    while(std::getline(line_stream, arg, ' ')) {
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
        std::getline(std::cin, line);
        shell_parse(line, args);
        status = shell_execute(args);

        line.clear();
        args.clear();
    } while (status);
}

int main()
{

    shell_loop();

    return EXIT_SUCCESS;
}