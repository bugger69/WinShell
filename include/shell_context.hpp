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

/* Tab SM macros*/
#define TAB_AUTOCOMP_NONE 0
#define TAB_AUTOCOMP_CMD 1
#define TAB_AUTOCOMP_PATH 2
#define TAB_AUTOCOMP_ARG 3

/* Tab state machine */
struct Tabsm {
public:
    int curr;
    int count;
    bool pathsPossible;
};

struct ShellContext {
public:
    /* Path contains the path to all executables, and names of shell commands too for more streamlined search */
    Trie* path;
    /* One will be needed for tracking current files */
    Trie* CurrDir;
    /* Temporary trie for path autocomplete */
    Trie* newDir;
    /* Tab state machine for autocomplete */
    Tabsm* tabSM;
};