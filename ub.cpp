#include "ub.hpp"
#include "util.hpp"
#include "util.hpp"

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



const std::string savedInputsPath = "fuzzed-tests/";

std::random_device rd;
std::mt19937 gen(rd());


std::vector<std::string> educatedInputs = {
    "p cnf 0 0",
	"p cnf 2147483647 1\n1 0", // Large number
	"p cnf 2147483648 1\n1 0", // Large number
	"p cnf -2147483647 1\n1 0", // Large signed number
	"p cnf -2147483648 1\n1 0", // Large signed number
	"p cnf 2 3000000000\n1 -2 0", // Malformed header
    "p cnf 2 2\n1 -2 0\n0", // Empty clause
    "p cnf 1 0", // Single variable with no clauses
    "p cnf 2 1\n1 -1 0", // Tautological clause,
    "p cnf 2 3\n1 -1 0\n0\n2 -2 0", // Empty clause + tautolological clause
    "p cnf 3 3\n1 -2 0\n2 -3 0\n3 -1 0", // Cyclic dependencies
    "p cnf 4 4\n1 2 0\n-1 3 0\n-2 -3 0\n2 -4 0", // Trigger backtracking
    "c This is a comment\np cnf 2 2\n1 -2 0\n\n2 -1 0\nc Another comment", // Comments and whitespace
    R"(p cnf 10 20\n!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~)", // Punctuations
    R"(p cnf 10 20\n\n\n992349abcdefghijklmnopqrstuvFGHIJKLMRSTUVWXYZ!\"#
       $%&'()*+,-./:;<=>?@[\]^_`{|}~\n0\n001234abcdefghijklmnopqrstFGHIJKLMN
       OPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~\n)", // Garbage strings
    "p cnf 10 20\n\n\n01234567890123456789" // Digits without whitespace
};


std::string createInput() {
    int numberOfFormulas = 100000;
    int numberOfLiterals = 999;
    int formulasWidth = 10;
    std::string cnf = "p cnf " + std::to_string(numberOfLiterals) + " " + std::to_string(numberOfFormulas) + "\n999 0\n";

    for (int i = 0; i < numberOfFormulas; ++i) {
        for (int j = 0; j < formulasWidth; ++j) {
            cnf += std::to_string(gen() % 2 ? '-' : ' ') + std::to_string((gen() % numberOfLiterals) + 1) + " ";
        }
        cnf += "0\n";
    }

    return cnf;
}


std::string createGarbage() {
    std::string cnf = "p cnf 10 20\n";

    while (cnf.size() < 20) {
        int choice = gen() % 6;
        switch (choice) {
            case 0: return cnf;
            case 1: cnf += R"(!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~)"; break;
            case 2: cnf += "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; break;
            case 3: cnf += "0123456789"; break;
            case 4: cnf += "0"; break;
            case 5: cnf += "\n"; break;
        }
    }

    return cnf;
}


std::string makeCnfFile(const std::string& cnf, int fileNumber) {
    std::string tempDirName = "ubTempFiles";
    std::string tempDirPath = tempDirName + "/";
    
    // Create the temporary directory if it doesn't exist
    system(("mkdir -p " + tempDirPath).c_str());

    std::string filename = "ub_cnf_" + std::to_string(fileNumber) + ".cnf";
    std::string tempFilePath = tempDirPath + filename;
    std::ofstream tempFile(tempFilePath);

    if (tempFile.is_open()) {
        tempFile << cnf;
        tempFile.close();
    } else {
        std::cerr << "Failed to open file for writing in temp file: " << tempFilePath << std::endl;
    }

    return tempFilePath;
}


void generateUB(const std::string& sutPath, const std::string& inputsPath, unsigned int seed, int maxIterations) {
    gen.seed(seed);
    std::uniform_int_distribution<std::mt19937::result_type> dist2(1, 2);
    int index = 0;
    int iterations = 0;

    while (iterations < maxIterations) {
        std::string cnf;

        if (index < educatedInputs.size()) {
            cnf = educatedInputs[index];
        } else {
            if (dist2(rd) == 1) {
                cnf = createInput();
            } else if (dist2(rd) == 1) {
                cnf = createGarbage();
            }
        }

        std::string inputFile = makeCnfFile(cnf, index);
        std::string output = runSUT(inputFile, sutPath);

        if (!output.empty() && checkUb(output)) {
            // Save only the inputs that caused errors
            saveInput(savedInputsPath, cnf, index, "ub");
        }

        // Remove the temp file created from makeCnfFile
        std::remove(inputFile.c_str());

        ++index;
        ++iterations;
    }
}