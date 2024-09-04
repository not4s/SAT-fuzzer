#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>

// Check if a directory exists at the given path
bool directoryExists(const std::string& path);

// Remove a directory and all its contents at the given path
void removeDirectory(const std::string& path);

// Create a directory at the specified path, removing it first if it exists
void createDirectory(const std::string& path);

// Save the generated CNF input to a file
void saveInput(const std::string& outputPath, const std::string& content, int id, const std::string& funcType);

// Save error details to a text file
void saveError(const std::string& outputPath, const std::string& content, int id, const std::string& funcType);

// Run the SAT solver (SUT) on a given CNF input and return the output
std::string runSUT(const std::string& inputFilename, const std::string& sutPath);

// Check the output from the SUT for signs of undefined behavior (UB)
int checkUb(const std::string& sutOutput);

// Evict files if the number of saved error files exceeds the defined limit
void evictFilesIfNeeded();

#endif // UTIL_HPP
