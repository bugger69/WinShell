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
#include <atomic>
#include <memory>
#include <thread>
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

// Output macros
#define SHELL_CMD_OUT_STD_DEF 0
#define SHELL_CMD_OUT_STD_FILE 1
#define SHELL_CMD_OUT_STD_APPEND 2
#define SHELL_CMD_OUT_ERR_DEF 0
#define SHELL_CMD_OUT_ERR_FILE 1
#define SHELL_CMD_OUT_ERR_APPEND 2

/* To store process Info */
struct ProcessInfo {
public:
    int pid;
    std::string status;
    std::string cmd;
};


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

/* To track builtin jobs */
struct BuiltinJob {
    std::string cmd;
    std::atomic<bool> done{false}; // to be accessed safely from multiple threads
    int exitCode = EXIT_FAILURE;
    std::thread worker;
};

/* To track jobs */
struct Jobs {
public:
    int processCount;
    int nextBuiltinJobId;
    std::map<int, ProcessInfo> processJobs;
    std::map<int, std::unique_ptr<BuiltinJob>> builtinJobs;
};

/* Output Target Struct */
class OutputTarget {
    public:
        int flag_std = SHELL_CMD_OUT_STD_DEF;
        int flag_err = SHELL_CMD_OUT_ERR_DEF;
        bool flag_pipe = false;
        std::string file_std;
        std::string file_err;
        HANDLE pipeWrite = INVALID_HANDLE_VALUE;
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
    /* To store newly created job process ids with shell job number */
    Jobs* jobs;
    /* To store output targets */
    OutputTarget* consoleOut;
};