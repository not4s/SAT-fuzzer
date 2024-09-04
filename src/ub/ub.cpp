#include "ub.hpp"
#include "util/util.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <regex>
#include <cstdlib>
#include <chrono>
#include <thread>

// Path to save inputs that triggered errors
const std::string savedInputsPath = "fuzzed-tests/";

// Random number generator
std::random_device rd;
std::mt19937 gen(rd());

// Predefined inputs known to trigger edge cases in SAT solvers
std::vector<std::string> educatedInputs = {
    "p cnf 0 0", // Empty formula
    "p cnf 2147483647 1\n1 0", // Large positive number
    "p cnf 2147483648 1\n1 0", // Exceeding 32-bit positive limit
    "p cnf -2147483647 1\n1 0", // Large negative number
    "p cnf -2147483648 1\n1 0", // Exceeding 32-bit negative limit
    "p cnf 2 3000000000\n1 -2 0", // Malformed header with large clause number
    "p cnf 2 2\n1 -2 0\n0", // Empty clause
    "p cnf 1 0", // Single variable with no clauses
    "p cnf 2 1\n1 -1 0", // Tautological clause
    "p cnf 2 3\n1 -1 0\n0\n2 -2 0", // Empty clause and tautological clause
    "p cnf 3 3\n1 -2 0\n2 -3 0\n3 -1 0", // Cyclic dependencies
    "p cnf 4 4\n1 2 0\n-1 3 0\n-2 -3 0\n2 -4 0", // Backtracking scenario
    "c This is a comment\np cnf 2 2\n1 -2 0\n\n2 -1 0\nc Another comment", // Comments and whitespace
    R"(p cnf 10 20\n!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~)", // Punctuations in formula
    R"(p cnf 10 20\n\n\n992349abcdefghijklmnopqrstuvFGHIJKLMRSTUVWXYZ!\"#
       $%&'()*+,-./:;<=>?@[\]^_`{|}~\n0\n001234abcdefghijklmnopqrstFGHIJKLMN
       OPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~\n)", // Garbage strings
    "p cnf 10 20\n\n\n01234567890123456789" // Digits without whitespace
};

// Generate a large CNF input with random literals
std::string createInput() {
    int numberOfFormulas = 100000;
    int numberOfLiterals = 999;
    int formulasWidth = 10;
    std::string cnf = "p cnf " + std::to_string(numberOfLiterals) + " " + std::to_string(numberOfFormulas) + "\n999 0\n";

    // Randomly generate formulas
    for (int i = 0; i < numberOfFormulas; ++i) {
        for (int j = 0; j < formulasWidth; ++j) {
            cnf += std::to_string(gen() % 2 ? '-' : ' ') + std::to_string((gen() % numberOfLiterals) + 1) + " ";
        }
        cnf += "0\n"; // End clause
    }

    return cnf;
}

// Generate a malformed CNF input with random characters and garbage data
std::string createGarbage() {
    std::string cnf = "p cnf 10 20\n";

    // Add garbage characters to the CNF
    while (cnf.size() < 20) {
        int choice = gen() % 6;
        switch (choice) {
            case 0: return cnf; // Stop adding when desired length is reached
            case 1: cnf += R"(!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~)"; break;
            case 2: cnf += "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; break;
            case 3: cnf += "0123456789"; break;
            case 4: cnf += "0"; break;
            case 5: cnf += "\n"; break;
        }
    }

    return cnf;
}

// Create a temporary CNF file for testing
std::string makeCnfFile(const std::string& cnf, int fileNumber) {
    std::string tempDirName = "ubTempFiles";
    std::string tempDirPath = tempDirName + "/";
    
    // Create temporary directory if it doesn't exist
    system(("mkdir -p " + tempDirPath).c_str());

    std::string filename = "ub_cnf_" + std::to_string(fileNumber) + ".cnf";
    std::string tempFilePath = tempDirPath + filename;
    std::ofstream tempFile(tempFilePath);

    if (tempFile.is_open()) {
        tempFile << cnf;
        tempFile.close();
    } else {
        std::cerr << "Failed to open file for writing: " << tempFilePath << std::endl;
    }

    return tempFilePath;
}

// Generate CNF inputs to trigger undefined behavior (UB) in the SAT solver
void generateUB(const std::string& sutPath, const std::string& inputsPath, unsigned int seed, int maxIterations) {
    gen.seed(seed);
    std::uniform_int_distribution<std::mt19937::result_type> dist2(1, 2); // Random choice between inputs
    int index = 0;
    int iterations = 0;

    // Loop to generate and test inputs
    while (iterations < maxIterations) {
        std::string cnf;

        // Use predefined educated inputs first
        if (index < educatedInputs.size()) {
            cnf = educatedInputs[index];
        } else {
            // Randomly generate new inputs
            if (dist2(rd) == 1) {
                cnf = createInput(); // Valid CNF input
            } else if (dist2(rd) == 1) {
                cnf = createGarbage(); // Garbage input
            }
        }

        // Create temporary file and run the SUT
        std::string inputFile = makeCnfFile(cnf, index);
        std::string output = runSUT(inputFile, sutPath);

        // Check for undefined behavior and save inputs that triggered errors
        if (!output.empty() && checkUb(output)) {
            saveInput(savedInputsPath, cnf, index, "ub");
        }

        // Remove the temporary file
        std::remove(inputFile.c_str());

        ++index;
        ++iterations;
    }
}
