#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "shell_context.hpp"
#include "utils.hpp"


void setConsoleOutputBuf(std::vector<std::string> &args, OutputTarget &consoleOut);
void shell_parse(const std::string &line, std::vector<std::string> &args, ShellContext &context);