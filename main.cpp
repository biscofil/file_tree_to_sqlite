#include <string>
#include <iostream>
#include <filesystem>
#include "json.hpp"

// for convenience
using json = nlohmann::json;

namespace fs = std::filesystem;

void analyzeFolder(std::string path, json &j) {

    for (const auto &entry : fs::directory_iterator(path)) {

        if (entry.is_directory()) {

            analyzeFolder(entry.path(), j);

        } else {

            //std::cout << entry.path() << "\t" << entry.file_size() << std::endl;
            j[entry.path()] = entry.file_size();

        }
    }
}

int main() {
    json j;
    std::string path = "./";
    analyzeFolder(path, j);

    // explicit conversion to string
    std::string s = j.dump();

    // serialization with pretty printing
    // pass in the amount of spaces to indent
    std::cout << j.dump(4) << std::endl;
}