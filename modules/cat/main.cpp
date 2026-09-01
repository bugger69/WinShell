#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

namespace fs = std::filesystem;

void no_arguments_case() { // TODO: merge these two cases into one function
    std::string line;
    while(std::getline(std::cin, line)) {
        std::cout << line << std::endl;
    }
}

void show_file_contents(std::vector<std::string> args) {
    try {
        for(auto it : args) {
            fs::path filepath(it);
            std::ifstream file(filepath);
            std::string line;

            if(!file.is_open()) throw std::runtime_error("File not found!!");

            while(std::getline(file, line)) {
                std::cout << line << std::endl;
            }
        }
    } catch (const std::runtime_error& e) {
        std::cout << "cat: " << e.what() << std::endl;
    }
}

int main (int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    args.size() > 0 ? show_file_contents(args) : no_arguments_case();

    return 0;
}