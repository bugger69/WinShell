#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "shell_read.hpp"
#include "parser.hpp"
#include "shell_execute.hpp"
#include "shell_cmds.hpp"
#include "utils.hpp"

void shell_loop(void)
{
    std::string line;
    std::vector<std::string> args;
    OutputTarget consoleOut;
    int status;

    do
    {
        std::cout << "> ";
        shell_read(line);
        shell_parse(line, args);
        status = shell_execute(args, consoleOut);

        line.clear();
        args.clear();
        // std::cin.ignore();
    } while (!status);
}
// TODO: implement type and echo commands too
int main()
{

    shell_loop(); // TODO: fix the > and 1> operators not working errors, try replacing cout with out..

    return EXIT_SUCCESS;
}