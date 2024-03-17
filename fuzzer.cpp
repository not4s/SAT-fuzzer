#include "ub.hpp"
#include "func.hpp"

#include <iostream>
#include <string>
#include <deque>
#include <chrono>
#include <algorithm>
#include <thread>
#include <dirent.h>

const std::string FUZZED_TESTS_DIR = "fuzzed-tests";

void checkAndCreateFuzzedTestsDir() {
    // Check if the directory exists
    if (std::remove(FUZZED_TESTS_DIR.c_str()) == 0) {
        // If it exists, attempt to remove it
        std::cerr << "Failed to remove existing directory: " << FUZZED_TESTS_DIR << std::endl;
    }

    // Create the directory
    if (std::system(("mkdir " + FUZZED_TESTS_DIR).c_str()) != 0) {
        std::cerr << "Failed to create directory: " << FUZZED_TESTS_DIR << std::endl;
    }
}


std::vector<std::string> listCnfFiles(const std::string& directoryPath) {
    std::vector<std::string> cnfFiles;
    DIR* dir = opendir(directoryPath.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) { // Check if it's a regular file
                std::string fileName = entry->d_name;
                if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".cnf") {
                    cnfFiles.push_back(directoryPath + "/" + fileName);
                }
            }
        }
        closedir(dir);
    }
    return cnfFiles;
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " /path/to/SUT /path/to/inputs seed" << std::endl;
        return 1; // Return an error code
    }

    std::string sutPath = argv[1];
    std::string inputsPath = argv[2];
    unsigned seed = std::stoi(argv[3]);

    std::vector<std::string> inputFiles = listCnfFiles(inputsPath);

    checkAndCreateFuzzedTestsDir();
    int savedTestsCount = 0;

    // Use the provided arguments in your functions
    generateUB(sutPath, inputsPath,seed,200);
    functionalTransform(sutPath, inputFiles, seed);
    // std::thread threadUb(generateUB, sutPath, inputsPath, seed);
    // std::thread threadFunc(functionalTransform, sutPath, inputsPath, seed);
    
    // Wait for both threads to complete their execution
    // threadUb.join();
    // threadFunc.join();

    return 0;
}