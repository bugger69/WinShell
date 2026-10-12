#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

namespace fs = std::filesystem;

void show_stream(std::istream& in) {
    std::string line;
    while(std::getline(in, line)) {
        std::cout << line << '\n';
    }
}

void show_file(std::vector<std::string> args) {
    try {
        for(auto it : args) {
            fs::path filepath(it);
            std::ifstream file(filepath);

            if(!file.is_open()) throw std::runtime_error("File not found!!");

            show_stream(file);
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "cat: " << e.what() << std::endl;
    }
}

int main (int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    args.size() > 0 ? show_file(args) : show_stream(std::cin);

    return 0;
}