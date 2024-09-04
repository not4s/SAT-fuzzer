#ifndef UB_HPP
#define UB_HPP

#include <string>
#include <vector>
#include <regex>

// Generate a large CNF input with random literals
std::string createInput();

// Generate a malformed CNF input with random garbage data
std::string createGarbage();

// Create a temporary CNF file from a given CNF string and return the file path
std::string makeCnfFile(const std::string& cnf, int fileNumber);

// Save a CNF input that triggered an error to a specified path
void saveInput(const std::string& outputPath, const std::string& content, int index, const std::string& prefix);

// Run the SAT solver (SUT) on the provided CNF input file and return the solver's output
std::string runSUT(const std::string& inputFilename, const std::string& sutPath);

// Check the SAT solver's output for signs of undefined behavior
int checkUb(const std::string& sutOutput);

// Main function to generate CNF inputs and test them on the SAT solver for undefined behavior
void generateUB(const std::string& sutPath, const std::string& inputsPath, unsigned int seed, int maxIterations);

#endif // UB_HPP
