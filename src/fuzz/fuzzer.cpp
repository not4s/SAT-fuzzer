#include "ub/ub.hpp"
#include "core/func.hpp"

#include <iostream>
#include <string>
#include <deque>
#include <chrono>
#include <algorithm>
#include <thread>
#include <dirent.h>

// Directory to store test cases that trigger errors
const std::string FUZZED_TESTS_DIR = "fuzzed-tests";

// Function to check if the "fuzzed-tests" directory exists, remove it if necessary, and then recreate it
void checkAndCreateFuzzedTestsDir() {
    // Remove existing fuzzed-tests directory, if present
    if (std::remove(FUZZED_TESTS_DIR.c_str()) == 0) {
        std::cerr << "Failed to remove existing directory: " << FUZZED_TESTS_DIR << std::endl;
    }

    // Create the fuzzed-tests directory
    if (std::system(("mkdir " + FUZZED_TESTS_DIR).c_str()) != 0) {
        std::cerr << "Failed to create directory: " << FUZZED_TESTS_DIR << std::endl;
    }
}

// Function to list all CNF files in the specified directory
std::vector<std::string> listCnfFiles(const std::string& directoryPath) {
    std::vector<std::string> cnfFiles;
    DIR* dir = opendir(directoryPath.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        // Iterate through the files in the directory
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) { // Check if it's a regular file
                std::string fileName = entry->d_name;
                // Add file to list if it's a CNF file
                if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".cnf") {
                    cnfFiles.push_back(directoryPath + "/" + fileName);
                }
            }
        }
        closedir(dir);
    }
    return cnfFiles;
}

// Main function to execute the fuzzer
int main(int argc, char* argv[]) {
    // Ensure the correct number of arguments are provided
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " /path/to/SUT /path/to/inputs seed" << std::endl;
        return 1; // Return an error code
    }

    // Parse command line arguments
    std::string sutPath = argv[1];  // Path to the SAT solver (SUT)
    std::string inputsPath = argv[2];  // Path to input CNF files
    unsigned seed = std::stoi(argv[3]);  // Seed for random number generation

    // Get list of CNF files from the inputs directory
    std::vector<std::string> inputFiles = listCnfFiles(inputsPath);

    // Ensure the fuzzed-tests directory exists
    checkAndCreateFuzzedTestsDir();

    // Run the UB generation and functional transformation processes
    generateUB(sutPath, inputsPath, seed, 200);  // Test for undefined behavior
    functionalTransform(sutPath, inputFiles, seed);  // Apply functional transformations

    // Optionally, run these functions in parallel using threads:
    // std::thread threadUb(generateUB, sutPath, inputsPath, seed);
    // std::thread threadFunc(functionalTransform, sutPath, inputFiles, seed);
    
    // Wait for both threads to finish execution:
    // threadUb.join();
    // threadFunc.join();

    return 0; // Return success code
}
