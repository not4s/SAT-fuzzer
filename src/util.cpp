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

std::map<std::string, std::vector<std::string>> errorFiles; // Maps error type to list of filenames
std::map<std::string, int> errorCounts; // Maps error type to count

int savedInputsId = 0;

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


bool directoryExists(const std::string& path) {
    #ifdef _WIN32
    return _access(path.c_str(), 0) == 0;
    #else
    return access(path.c_str(), F_OK) == 0;
    #endif
}


void removeDirectory(const std::string& path) {
    std::string command;
    #ifdef _WIN32
    command = "rmdir /s /q " + path; // Windows command to remove directory recursively
    #else
    command = "rm -rf " + path; // Unix command to remove directory recursively
    #endif
    system(command.c_str());
}


void createDirectory(const std::string& path) {
    if (directoryExists(path)) {
        removeDirectory(path);
    }

    #ifdef _WIN32
        if (_mkdir(path.c_str()) != 0) {
    #else
        if (mkdir(path.c_str(), 0755) != 0) {
    #endif
            std::cerr << "Error creating directory: " << path << std::endl;
        }
}


void saveInput(const std::string& outputPath, const std::string& content, int id, const std::string& funcType) {
    std::ofstream outFile(outputPath + funcType + "_" + std::to_string(id) + ".cnf");

    if (outFile) {
        outFile << content;
    } else {
        std::cerr << "Failed to save input: " << outputPath << std::endl;
    }
}


void saveError(const std::string& outputPath, const std::string& content, int id, const std::string& funcType) {
    std::ofstream outFile(outputPath + funcType + "_" + std::to_string(id) + ".txt");

    if (outFile) {
        outFile << content;
    } else {
        std::cerr << "Failed to save error: " << outputPath << std::endl;
    }
}


std::string runSUT(const std::string& inputFilename, const std::string& sutPath) {
    // Construct the command to run the script with redirection of stdout and stderr to files
    std::string baseName = inputFilename.substr(0, inputFilename.size() - 4);
    std::string outputFilename = baseName + ".out";
    std::string errorFilename = baseName + ".err";
    std::string command = sutPath + "/runsat.sh " + inputFilename + " > " + outputFilename + " 2> " + errorFilename;

    // Execute the command
    int result = std::system(command.c_str());

    // Read the error output from the file
    std::ifstream errorFile(errorFilename);
    std::stringstream errorStream;

    errorStream << errorFile.rdbuf();
    auto errorOutput = errorStream.str();

    // std::ifstream outputFile(outputFilename);
    // std::string line;

    // while (getline(outputFile, line)) {
    //     std::cout << line << std::endl;
    // }

    // std::filesystem::remove(outputFilename);
    // std::filesystem::remove(errorFilename);

	return errorOutput;
}


int checkUb(const std::string& sutOutput) {
    int ubs = 0;
    std::istringstream stream(sutOutput);
    std::string line;

    while (std::getline(stream, line)) {
        for (const auto& regex : errorFormats) {
            if (std::regex_search(line, regex)) {
                std::cout << line << std::endl;
                ubs++;
            }
        }
    }

    return ubs;
}


void evictFilesIfNeeded() {
    const int MaxFiles = 20; // Adjust as needed for your specific requirement
    while (std::accumulate(errorCounts.begin(), errorCounts.end(), 0,
                            [](int acc, const auto& entry) { return acc + entry.second; }) > MaxFiles) {
        // Find the error type with the highest count
        auto mostFrequent = std::max_element(errorCounts.begin(), errorCounts.end(),
                                             [](const auto& a, const auto& b) { return a.second < b.second; });
        if (mostFrequent == errorCounts.end() || mostFrequent->second == 0) break; // Safety check

        // Identify and remove the oldest file associated with this error type
        if (!errorFiles[mostFrequent->first].empty()) {
            std::string fileToRemove = errorFiles[mostFrequent->first].front();
            std::remove(fileToRemove.c_str()); // Remove the file from the filesystem
            errorFiles[mostFrequent->first].erase(errorFiles[mostFrequent->first].begin()); // Remove from tracking

            // Update the counts
            mostFrequent->second--;
            if (mostFrequent->second == 0) {
                // Clean up if no more files are associated with this error type
                errorFiles.erase(mostFrequent->first);
            }
        }
    }
}