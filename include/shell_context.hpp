#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <map>
#include <windows.h>
#include <algorithm>
#include "Trie.hpp"

/* Tab SM macros*/
#define TAB_AUTOCOMP_NONE 0
#define TAB_AUTOCOMP_CMD 1
#define TAB_AUTOCOMP_PATH 2
#define TAB_AUTOCOMP_ARG 3

/* Complete builtin autocomplete macros*/
#define COMPLETE_AUTOCOMPLETE_DEFAULT 0
#define COMPLETE_AUTOCOMPLETE_FILE 1
#define COMPLETE_AUTOCOMPLETE_DIRECTORY 2
#define COMPLETE_AUTOCOMPLETE_FUNCTION 3
#define COMPLETE_AUTOCOMPLETE_SCRIPT 4


/* Tab state machine */
struct Tabsm {
public:
    int curr;
    int count;
    bool pathsPossible;
};

/* Complete Info Struct */
struct CompleteInfo {
public:
    int compFlag;
    std::string execPath;
    std::vector<std::string> currCompSet;
    Trie* autoCompInfo;
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
    /* Map of commands for command arguments autocomplete */
    std::map<std::string, CompleteInfo*> cmdComp;
};