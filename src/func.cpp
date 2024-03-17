#include "func.hpp"
#include "util.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <random>
#include <algorithm>
#include <string>
#include <deque>

const std::string FUNC_TESTS = "func-tests";
const int NUM_OF_TRANSFORMS = 16;
const std::string SAT_UNKNOWN__UNSAT_UNKNOWN = "SAT->UNKNOWN\nUNSAT->UNKNOWN\n";
const std::string SAT_UNKNOWN__UNSAT_UNSAT = "SAT->UNKNOWN\nUNSAT->UNSAT\n";
const std::string SAT_SAT__UNSAT_UNKOWN = "SAT->SAT\nUNSAT->UNKNOWN\n";
const std::string SAT_SAT__UNSAT_UNSAT = "SAT->SAT\nUNSAT->UNSAT\n";
const std::string SAT_UNSAT__UNSAT_UNSAT = "SAT->UNSAT\nUNSAT->UNSAT\n";

std::mt19937 generator;

// Each line is represented as a vector of string (literals)
using FormulaType = std::vector<std::vector<std::string>>;

FormulaType swapLiterals(const FormulaType& formula) {
    FormulaType newFormula;

    for (auto const& clause : formula) {
        std::vector<std::string> newClause(clause.begin(), clause.end() - 1);
        std::shuffle(newClause.begin(), newClause.end(), generator);
        newClause.emplace_back(std::to_string(0));

        newFormula.emplace_back(newClause);
    }

    return newFormula;
}

std::pair<FormulaType, int> addClauses(int numOfVars, int numOfClauses, const FormulaType& formula) {
    FormulaType newFormula;

    for (auto const& clause : formula) {
        std::vector<std::string> newClause(clause);
        newFormula.emplace_back(newClause);
    }

    int numOfNewClauses = std::uniform_int_distribution<>(0, numOfClauses)(generator);

    for (int i = 0; i < numOfNewClauses; ++i) {
        std::vector<std::string> newClause;
        int clauseSize = std::uniform_int_distribution<>(1, 10)(generator);

        for (int j = 0; j < clauseSize; ++j) {
            int var = std::uniform_int_distribution<>(1, numOfVars)(generator);
            newClause.emplace_back(std::to_string(var));
        }

        newClause.emplace_back(std::to_string(0));
        newFormula.emplace_back(newClause);
    }

    return std::make_pair(newFormula, numOfNewClauses);
}


// Function to add extreme value clauses to a formula
std::pair<FormulaType, int> addExtremeValueClauses(const FormulaType& formula, int numOfVars) {
    FormulaType newFormula = formula; // Copy the original formula

    // Add clauses with extremely large positive and negative integer values
    std::vector<std::string> extremePositiveClause = {std::to_string(std::numeric_limits<int>::max()), "0"};
    std::vector<std::string> extremeNegativeClause = {std::to_string(std::numeric_limits<int>::min()), "0"};

    // Adding extreme value clauses to the new formula
    newFormula.push_back(extremePositiveClause);
    newFormula.push_back(extremeNegativeClause);

    // Return the new formula with added clauses, the updated number of clauses, and a tag indicating the type of transformation
    return std::make_pair(newFormula, newFormula.size());
}


std::pair<FormulaType, int> deleteClauses(int numOfClauses, const FormulaType& formula) {
    int numOfDeletedClauses = std::uniform_int_distribution<>(0, numOfClauses)(generator);
    FormulaType newFormula(formula.begin(), formula.end() - numOfDeletedClauses);

    return std::make_pair(newFormula, numOfDeletedClauses);
}


FormulaType swapClauses(const FormulaType& formula) {
    FormulaType newFormula(formula);
    std::shuffle(newFormula.begin(), newFormula.end(), generator);

    return newFormula;
}


FormulaType clauseNegation(const FormulaType& formula) {
    FormulaType newFormula(formula);

    for (auto& clause : newFormula) {
        for (auto& literal : clause) {
            if (literal != "0") {
                literal = (literal[0] == '-') ? literal.substr(1) : "-" + literal;
            }
        }
    }

    return newFormula;
}

FormulaType addEmptyClause(const FormulaType& formula) {
    FormulaType newFormula(formula);

    newFormula.push_back({std::to_string(0)});  // empty clause

    return newFormula;
}


FormulaType addTautology(const FormulaType& formula) {
    FormulaType newFormula(formula);

    newFormula.push_back({"1", "-1", "0"});  // tautological clause

    return newFormula;
}


std::tuple<FormulaType, int, std::string> transform(const FormulaType& formula, int numOfClauses, int numOfVars) {
    std::tuple<FormulaType, int, std::string> transformedFormula;
	std::uniform_int_distribution<int> gen(0, NUM_OF_TRANSFORMS); // Adjust the range based on the number of transformations
    int transformationChoice = gen(generator);
	FormulaType newFormula;

	switch (transformationChoice) {
		case 0:
			// Swapping Clauses
			newFormula = swapClauses(formula);
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_SAT__UNSAT_UNSAT);
			break;
		case 1:
			// Swapping Literals
			newFormula = swapLiterals(formula);
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_SAT__UNSAT_UNSAT);
			break;
		case 2:
		{
			// Adding Clauses
			auto clausesAdded = addClauses(numOfVars, numOfClauses, formula);
			newFormula = clausesAdded.first;
			auto numOfAddedClauses = clausesAdded.second;

			transformedFormula = std::make_tuple(newFormula, numOfAddedClauses + numOfClauses, SAT_UNKNOWN__UNSAT_UNSAT);
			break;
		}
		case 3:
		{
			// Deleting Clauses
			auto clausesDeleted = deleteClauses(numOfClauses, formula);
			newFormula = clausesDeleted.first;
			auto numOfDeletedClauses = clausesDeleted.second;

			transformedFormula = std::make_tuple(newFormula, numOfClauses - numOfDeletedClauses, SAT_SAT__UNSAT_UNKOWN);
			break;
		}
		case 4:
			// Swapping Clauses
			newFormula = swapLiterals(swapClauses(formula));
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_SAT__UNSAT_UNSAT);
			break;
		case 5:
		{
			// Swapping Clauses then Adding Clauses
			auto clausesAdded = addClauses(numOfVars, numOfClauses, swapClauses(formula));
			newFormula = clausesAdded.first;
			auto numOfAddedClauses = clausesAdded.second;

			transformedFormula = std::make_tuple(newFormula, numOfAddedClauses + numOfClauses, SAT_UNKNOWN__UNSAT_UNSAT);
			break;
		}
		case 6:
		{
			// Swapping Clauses then Deleting Clauses
			auto clausesDeleted = deleteClauses(numOfClauses, swapClauses(formula));
			newFormula = clausesDeleted.first;
			auto numOfDeletedClauses = clausesDeleted.second;

			transformedFormula = std::make_tuple(newFormula, numOfClauses - numOfDeletedClauses, SAT_SAT__UNSAT_UNKOWN);
			break;
		}
		case 7:
		{
			// Swapping Clauses, Swapping Literals, then Adding Clauses
			auto clausesAdded = addClauses(numOfVars, numOfClauses, swapLiterals(swapClauses(formula)));
			newFormula = clausesAdded.first;
			auto numOfAddedClauses = clausesAdded.second;

			transformedFormula = std::make_tuple(newFormula, numOfAddedClauses + numOfClauses, SAT_UNKNOWN__UNSAT_UNSAT);
			break;
		}
		case 8:
		{
			// Swapping Clauses, Swapping Literals, then Deleting Clauses
			auto clausesDeleted = deleteClauses(numOfClauses, swapLiterals(swapClauses(formula)));
			newFormula = clausesDeleted.first;
			auto numOfDeletedClauses = clausesDeleted.second;

			transformedFormula = std::make_tuple(newFormula, numOfClauses - numOfDeletedClauses, SAT_SAT__UNSAT_UNKOWN);
			break;
		}
		case 9:
		{
			// Swapping Literals then Deleting Clauses
			auto clausesDeleted = deleteClauses(numOfClauses, swapLiterals(formula));
			newFormula = clausesDeleted.first;
			auto numOfDeletedClauses = clausesDeleted.second;

			transformedFormula = std::make_tuple(newFormula, numOfClauses - numOfDeletedClauses, SAT_SAT__UNSAT_UNKOWN);
			break;
		}
		case 10:
		{
			// Negating Clauses
			newFormula = clauseNegation(formula);
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_UNKNOWN__UNSAT_UNKNOWN);
			break;
		}
		case 11:
			// Swapping Literals then Negating Clauses
			newFormula = clauseNegation(swapLiterals(formula));
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_UNKNOWN__UNSAT_UNKNOWN);
			break;
		case 12:
			// Swapping Literals, Negating Clauses, then Swapping Clauses
			newFormula = swapClauses(clauseNegation(swapLiterals(formula)));
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_UNKNOWN__UNSAT_UNKNOWN);
			break;
		case 13:
			// Add Empty Clause
			newFormula = addEmptyClause(formula);
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_UNKNOWN__UNSAT_UNKNOWN);
			break;
		case 14:
			// Add Tautology
			newFormula = addTautology(formula);
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_UNSAT__UNSAT_UNSAT);
			break;
		case 15:
			// Add Empty Clause and a Tautology
			newFormula = addEmptyClause(formula);
			transformedFormula = std::make_tuple(newFormula, numOfClauses, SAT_UNSAT__UNSAT_UNSAT);
			break;
		case 16:
		{
			// Adding Clauses
			auto clausesAdded = addExtremeValueClauses(formula, numOfClauses);
			newFormula = clausesAdded.first;

			transformedFormula = std::make_tuple(newFormula, clausesAdded.second, SAT_UNKNOWN__UNSAT_UNKNOWN);
			break;
		}
		default:
			transformedFormula = std::make_tuple(formula, numOfClauses, "Original");
			break;
	}
    

    return transformedFormula;
}


void functionalTransform(const std::string& sutPath, const std::vector<std::string>& inputFiles, unsigned int seed) {
    std::mt19937 generator(seed);
	std::deque<std::tuple<FormulaType, int, int, std::string>> files;

    for (const auto& inputFilename : inputFiles) {
        std::ifstream formulaFile(inputFilename);
        std::string firstLine;
        getline(formulaFile, firstLine);

        int numOfVars, numOfClauses;
        std::stringstream ss(firstLine);
        std::string p, cnf;

        ss >> p >> cnf >> numOfVars >> numOfClauses;

        FormulaType clauses;
        std::string line;

        while (getline(formulaFile, line)) {
            std::istringstream iss(line);
            std::vector<std::string> clause;
            std::string lit;

            while (iss >> lit) {
                clause.emplace_back(lit);
            }

            clauses.emplace_back(clause);
        }

		files.emplace_back(std::make_tuple(clauses, numOfClauses, numOfVars, inputFilename));
	}
		
	createDirectory(FUNC_TESTS);

	int index = 0;

	while (true) {
		auto file = files.front(); files.pop_front();
		auto clauses = std::get<0>(file);
		auto numOfClauses = std::get<1>(file);
		auto numOfVars = std::get<2>(file);
		auto inputFilename = std::get<3>(file);

        auto transformation = transform(clauses, numOfClauses, numOfVars);

		auto [newFormula, newNumOfClauses, satString] = transformation;

		std::string cnfFilename = FUNC_TESTS + "/" + std::to_string(index) + ".cnf";
		std::string txtFilename = FUNC_TESTS + "/" + std::to_string(index) + ".txt";

		std::stringstream cnfStream;
		std::ofstream newCnfFile(cnfFilename);

		cnfStream << "p cnf " << numOfVars << " " << newNumOfClauses << "\n";
		
		for (const auto& line : newFormula) {
			for (auto lit : line) {
				cnfStream << lit << " ";
			}

			cnfStream << "\n";
		}

		if (!newCnfFile.is_open()) {
			std::cerr << "Failed to open CNF file in func for writing: " << cnfFilename << std::endl;
			continue;
		}

		std::string cnf = cnfStream.str();
		newCnfFile << cnf;

		std::ofstream newTxtFile(txtFilename);

		if (!newTxtFile.is_open()) {
			std::cerr << "Failed to open func txt file for writing: " << txtFilename << std::endl;
			continue;
		}

		newTxtFile << satString;

		std::string output = runSUT(cnfFilename, sutPath);

		if (!output.empty() && checkUb(output)) {
			saveInput("fuzzed-tests/", cnf, index, "func");
			// saveError("fuzzed-tests/", output, index, "func");
		}

		files.emplace_back(std::make_tuple(newFormula, newNumOfClauses, numOfVars, cnfFilename));

		// std::remove(cnfFilename.c_str());
		// std::remove(txtFilename.c_str());

		// evictFilesIfNeeded();

		++index;
	}
}