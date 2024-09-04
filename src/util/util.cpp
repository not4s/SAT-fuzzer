#include "util.hpp"

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <system_error>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h> // For _mkdir on Windows
    #include <io.h> // For access on Windows
#else
    #include <unistd.h> // For access and rmdir on Unix
#endif

// Maps error types to filenames and counts for tracking errors
std::map<std::string, std::vector<std::string>> errorFiles;
std::map<std::string, int> errorCounts;

int savedInputsId = 0;

// List of regular expressions to match various error patterns
std::vector<std::regex> errorFormats = {
	std::regex(".*runtime error.*"),
	std::regex(".*runtime.+negation"),
    std::regex(".*runtime.+null pointer"),
    std::regex(".*runtime.+shift"),
    std::regex(".*runtime.+signed integer"),
    std::regex(".*undefined behavior.*"),
    std::regex(".*floating-point exception.*"),
    std::regex(".*segmentation fault.*"),
    std::regex(".*invalid memory reference.*"),
    std::regex(".*stack-overflow.*"),
    std::regex(".*double-free.*"),
    std::regex(".*memory leak.*"),
    std::regex(".*data race.*"),
    std::regex(".*unreachable code.*"),
    std::regex(".*out-of-bounds.*"),
    std::regex("==.*UndefinedBehaviorSanitizer.*"),
    std::regex("==.*AddressSanitizer: heap-use-after-free"),
    std::regex("==.*AddressSanitizer: heap-buffer-overflow"),
    std::regex("==.*AddressSanitizer: stack-buffer-overflow")
};

// Check if a directory exists
bool directoryExists(const std::string& path) {
    #ifdef _WIN32
    return _access(path.c_str(), 0) == 0;
    #else
    return access(path.c_str(), F_OK) == 0;
    #endif
}

// Remove a directory recursively
void removeDirectory(const std::string& path) {
    std::string command;
    #ifdef _WIN32
    command = "rmdir /s /q " + path; // Windows command
    #else
    command = "rm -rf " + path; // Unix command
    #endif
    system(command.c_str());
}

// Create a directory, removing it first if it exists
void createDirectory(const std::string& path) {
    if (directoryExists(path)) {
        removeDirectory(path); // Remove if directory exists
    }

    // Create the directory
    #ifdef _WIN32
        if (_mkdir(path.c_str()) != 0) {
    #else
        if (mkdir(path.c_str(), 0755) != 0) {
    #endif
            std::cerr << "Error creating directory: " << path << std::endl;
        }
}

// Save a generated CNF input to a file
void saveInput(const std::string& outputPath, const std::string& content, int id, const std::string& funcType) {
    std::ofstream outFile(outputPath + funcType + "_" + std::to_string(id) + ".cnf");
    if (outFile) {
        outFile << content;
    } else {
        std::cerr << "Failed to save input: " << outputPath << std::endl;
    }
}

// Save error details to a text file
void saveError(const std::string& outputPath, const std::string& content, int id, const std::string& funcType) {
    std::ofstream outFile(outputPath + funcType + "_" + std::to_string(id) + ".txt");
    if (outFile) {
        outFile << content;
    } else {
        std::cerr << "Failed to save error: " << outputPath << std::endl;
    }
}

// Run the SAT solver (SUT) on a given CNF input and capture the output
std::string runSUT(const std::string& inputFilename, const std::string& sutPath) {
    // Construct the command to run SUT and redirect stdout and stderr
    std::string baseName = inputFilename.substr(0, inputFilename.size() - 4);
    std::string outputFilename = baseName + ".out";
    std::string errorFilename = baseName + ".err";
    std::string command = sutPath + "/runsat.sh " + inputFilename + " > " + outputFilename + " 2> " + errorFilename;

    // Execute the command
    std::system(command.c_str());

    // Read the error output from the generated error file
    std::ifstream errorFile(errorFilename);
    std::stringstream errorStream;
    errorStream << errorFile.rdbuf();
    auto errorOutput = errorStream.str();

    return errorOutput;
}

// Check the output from the SUT for any signs of undefined behavior (UB) using regex patterns
int checkUb(const std::string& sutOutput) {
    int ubs = 0;
    std::istringstream stream(sutOutput);
    std::string line;

    // Search each line for error patterns
    while (std::getline(stream, line)) {
        for (const auto& regex : errorFormats) {
            if (std::regex_search(line, regex)) {
                std::cout << line << std::endl; // Output the matched error
                ubs++;
            }
        }
    }

    return ubs; // Return the count of UB instances detected
}

// Evict old files from error logs if the number of files exceeds the limit
void evictFilesIfNeeded() {
    const int MaxFiles = 20; // Maximum number of files to retain
    // Calculate total number of error files
    while (std::accumulate(errorCounts.begin(), errorCounts.end(), 0,
                            [](int acc, const auto& entry) { return acc + entry.second; }) > MaxFiles) {
        // Find the error type with the highest count
        auto mostFrequent = std::max_element(errorCounts.begin(), errorCounts.end(),
                                             [](const auto& a, const auto& b) { return a.second < b.second; });
        if (mostFrequent == errorCounts.end() || mostFrequent->second == 0) break; // Safety check

        // Remove the oldest file associated with this error type
        if (!errorFiles[mostFrequent->first].empty()) {
            std::string fileToRemove = errorFiles[mostFrequent->first].front();
            std::remove(fileToRemove.c_str()); // Remove file from filesystem
            errorFiles[mostFrequent->first].erase(errorFiles[mostFrequent->first].begin()); // Remove from tracking

            // Update error count
            mostFrequent->second--;
            if (mostFrequent->second == 0) {
                errorFiles.erase(mostFrequent->first); // Clean up if no files remain for this error type
            }
        }
    }
}
