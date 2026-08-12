#include <iostream>
#include <string>
#include <vector>

int shell_execute(std::vector<std::string> &args)
{
    return EXIT_SUCCESS;
}

void shell_parse(std ::string &line, std::vector<std::string> &args)
{
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