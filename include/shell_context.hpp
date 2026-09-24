#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "Trie.hpp"

struct ShellContext {
public:
    /* Path contains the path to all executables, and names of shell commands too for more streamlined search */
    Trie* path;
    /* Shell Commands have only shell commands for easier search later on*/
    Trie* shellCmds;
};