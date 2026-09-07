cmake_minimum_required(VERSION 3.20)

project(WinShell)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)

add_executable(WinShell src/main.cpp src/shell_cmds.cpp src/utils.cpp src/parser.cpp src/shell_execute.cpp)

target_include_directories(WinShell PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(modules)