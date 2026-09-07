#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "utils.hpp"
#include "shell_cmds.hpp"

void setConsoleOutputBuf(std::vector<std::string> &args, OutputTarget &consoleOut);
std::string shell_find_exec(std::vector<std::string> &args, int &found);
int shell_process_launch(std::vector<std::string> &args, STARTUPINFOW &si, PROCESS_INFORMATION &pi, OutputTarget &consoleOut, std::ostream* out, std::ostream* err);
int shell_execute(std::vector<std::string> &args, OutputTarget &consoleOut);