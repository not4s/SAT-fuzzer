#include "func.hpp"
#include "util/util.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <random>
#include <algorithm>
#include <string>
#include <deque>

// Constants for transformations and result status
const std::string FUNC_TESTS = "func-tests";
const int NUM_OF_TRANSFORMS = 16;
const std::string SAT_UNKNOWN__UNSAT_UNKNOWN = "SAT->UNKNOWN\nUNSAT->UNKNOWN\n";
const std::string SAT_UNKNOWN__UNSAT_UNSAT = "SAT->UNKNOWN\nUNSAT->UNSAT\n";
const std::string SAT_SAT__UNSAT_UNKOWN = "SAT->SAT\nUNSAT->UNKNOWN\n";
const std::string SAT_SAT__UNSAT_UNSAT = "SAT->SAT\nUNSAT->UNSAT\n";
const std::string SAT_UNSAT__UNSAT_UNSAT = "SAT->UNSAT\nUNSAT->UNSAT\n";

std::mt19937 generator; // Random number generator for transformations

// Define formula type as a vector of vector of strings (literals)
using FormulaType = std::vector<std::vector<std::string>>;

// Swap literals within each clause
FormulaType swapLiterals(const FormulaType& formula) {
    FormulaType newFormula;

    for (auto const& clause : formula) {
        std::vector<std::string> newClause(clause.begin(), clause.end() - 1); // Exclude the ending '0'
        std::shuffle(newClause.begin(), newClause.end(), generator); // Random shuffle
        newClause.emplace_back(std::to_string(0)); // Re-append '0'
        newFormula.emplace_back(newClause);
    }

    return newFormula;
}

// Add random clauses to the formula
std::pair<FormulaType, int> addClauses(int numOfVars, int numOfClauses, const FormulaType& formula) {
    FormulaType newFormula = formula; // Copy original formula

    // Generate random number of new clauses
    int numOfNewClauses = std::uniform_int_distribution<>(0, numOfClauses)(generator);

    for (int i = 0; i < numOfNewClauses; ++i) {
        std::vector<std::string> newClause;
        int clauseSize = std::uniform_int_distribution<>(1, 10)(generator); // Random clause size

        // Add random literals to the clause
        for (int j = 0; j < clauseSize; ++j) {
            int var = std::uniform_int_distribution<>(1, numOfVars)(generator);
            newClause.emplace_back(std::to_string(var));
        }

        newClause.emplace_back(std::to_string(0)); // End clause with '0'
        newFormula.emplace_back(newClause);
    }

    return std::make_pair(newFormula, numOfNewClauses);
}

// Add extreme value clauses (max/min integer literals)
std::pair<FormulaType, int> addExtremeValueClauses(const FormulaType& formula, int numOfVars) {
    FormulaType newFormula = formula; // Copy original formula

    // Add clauses with extreme positive and negative values
    std::vector<std::string> extremePositiveClause = {std::to_string(std::numeric_limits<int>::max()), "0"};
    std::vector<std::string> extremeNegativeClause = {std::to_string(std::numeric_limits<int>::min()), "0"};

    newFormula.push_back(extremePositiveClause);
    newFormula.push_back(extremeNegativeClause);

    return std::make_pair(newFormula, newFormula.size());
}

// Delete random number of clauses from the formula
std::pair<FormulaType, int> deleteClauses(int numOfClauses, const FormulaType& formula) {
    int numOfDeletedClauses = std::uniform_int_distribution<>(0, numOfClauses)(generator);
    FormulaType newFormula(formula.begin(), formula.end() - numOfDeletedClauses); // Remove last clauses

    return std::make_pair(newFormula, numOfDeletedClauses);
}

// Shuffle the order of clauses in the formula
FormulaType swapClauses(const FormulaType& formula) {
    FormulaType newFormula(formula);
    std::shuffle(newFormula.begin(), newFormula.end(), generator); // Random shuffle of clauses

    return newFormula;
}

// Negate literals in the formula
FormulaType clauseNegation(const FormulaType& formula) {
    FormulaType newFormula = formula;

    for (auto& clause : newFormula) {
        for (auto& literal : clause) {
            if (literal != "0") {
                literal = (literal[0] == '-') ? literal.substr(1) : "-" + literal; // Negate literal
            }
        }
    }

    return newFormula;
}

// Add an empty clause (unsatisfiable formula)
FormulaType addEmptyClause(const FormulaType& formula) {
    FormulaType newFormula = formula;
    newFormula.push_back({std::to_string(0)}); // Add empty clause
    return newFormula;
}

// Add a tautological clause (always true)
FormulaType addTautology(const FormulaType& formula) {
    FormulaType newFormula = formula;
    newFormula.push_back({"1", "-1", "0"}); // Add tautological clause
    return newFormula;
}

// Perform a random transformation on the formula
std::tuple<FormulaType, int, std::string> transform(const FormulaType& formula, int numOfClauses, int numOfVars) {
    std::tuple<FormulaType, int, std::string> transformedFormula;
	std::uniform_int_distribution<int> gen(0, NUM_OF_TRANSFORMS); // Random transformation choice
    int transformationChoice = gen(generator);
	FormulaType newFormula;

	// Apply selected transformation
	switch (transformationChoice) {
		case 0:
			newFormula = swapClauses(formula);
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_SAT__UNSAT_UNSAT);
			break;
		case 1:
			newFormula = swapLiterals(formula);
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_SAT__UNSAT_UNSAT);
			break;
		case 2:
		{
			auto clausesAdded = addClauses(numOfVars, numOfClauses, formula);
			newFormula = clausesAdded.first;
			transformedFormula = std::make_tuple(newFormula, clausesAdded.second + numOfClauses, SAT_UNKNOWN__UNSAT_UNSAT);
			break;
		}
		case 3:
		{
			auto clausesDeleted = deleteClauses(numOfClauses, formula);
			newFormula = clausesDeleted.first;
			transformedFormula = std::make_tuple(newFormula, numOfClauses - clausesDeleted.second, SAT_SAT__UNSAT_UNKOWN);
			break;
		}
		// Additional cases for transformations...
		default:
			transformedFormula = std::make_tuple(formula, numOfClauses, "Original");
			break;
	}

    return transformedFormula;
}

// Main function to apply functional transformations on input files
void functionalTransform(const std::string& sutPath, const std::vector<std::string>& inputFiles, unsigned int seed) {
    std::mt19937 generator(seed);
	std::deque<std::tuple<FormulaType, int, int, std::string>> files;

    // Load and parse input CNF files
    for (const auto& inputFilename : inputFiles) {
        std::ifstream formulaFile(inputFilename);
        std::string firstLine;
        getline(formulaFile, firstLine);

        int numOfVars, numOfClauses;
        std::stringstream ss(firstLine);
        ss >> std::ws; // Skip whitespace
        ss >> numOfVars >> numOfClauses; // Extract variables and clauses

        FormulaType clauses;
        std::string line;

        while (getline(formulaFile, line)) {
            std::istringstream iss(line);
            std::vector<std::string> clause;
            std::string lit;
            while (iss >> lit) {
                clause.emplace_back(lit); // Parse literals into clauses
            }
            clauses.emplace_back(clause);
        }
		files.emplace_back(std::make_tuple(clauses, numOfClauses, numOfVars, inputFilename));
	}
		
	createDirectory(FUNC_TESTS); // Create directory for test cases

	int index = 0;

	// Main loop to process files and apply transformations
	while (true) {
		auto file = files.front(); files.pop_front();
		auto clauses = std::get<0>(file);
		auto numOfClauses = std::get<1>(file);
		auto numOfVars = std::get<2>(file);

        auto transformation = transform(clauses, numOfClauses, numOfVars);
		auto [newFormula, newNumOfClauses, satString] = transformation;

		// Generate output filenames
		std::string cnfFilename = FUNC_TESTS + "/" + std::to_string(index) + ".cnf";
		std::string txtFilename = FUNC_TESTS + "/" + std::to_string(index) + ".txt";

		std::stringstream cnfStream;
		std::ofstream newCnfFile(cnfFilename);

		cnfStream << "p cnf " << numOfVars << " " << newNumOfClauses << "\n";
		for (const auto& line : newFormula) {
			for (const auto& lit : line) {
				cnfStream << lit << " ";
			}
			cnfStream << "\n";
		}

		if (!newCnfFile.is_open()) {
			std::cerr << "Failed to open CNF file: " << cnfFilename << std::endl;
			continue;
		}
		newCnfFile << cnfStream.str();

		std::ofstream newTxtFile(txtFilename);
		if (!newTxtFile.is_open()) {
			std::cerr << "Failed to open TXT file: " << txtFilename << std::endl;
			continue;
		}
		newTxtFile << satString;

		// Run the SUT on the generated formula
		std::string output = runSUT(cnfFilename, sutPath);

		// Check for undefined behavior
		if (!output.empty() && checkUb(output)) {
			saveInput("fuzzed-tests/", cnfStream.str(), index, "func");
		}

		files.emplace_back(std::make_tuple(newFormula, newNumOfClauses, numOfVars, cnfFilename));

		++index;
	}
}
