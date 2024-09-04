#ifndef FUNC_HPP
#define FUNC_HPP

#include <vector>
#include <string>
#include <random>
#include <utility>
#include <tuple>
#include <fstream>

// Type definition for representing a formula as a list of clauses
using FormulaType = std::vector<std::vector<std::string>>;

// External constants representing various SAT/UNSAT result statuses
extern const std::string FUNC_TESTS;
extern const std::string SAT_UNKNOWN__UNSAT_UNKNOWN;
extern const std::string SAT_UNKNOWN__UNSAT_UNSAT;
extern const std::string SAT_SAT__UNSAT_UNKOWN;
extern const std::string SAT_SAT__UNSAT_UNSAT;
extern const std::string SAT_UNSAT__UNSAT_UNSAT;

// Function to randomly swap literals within each clause of the formula
FormulaType swapLiterals(const FormulaType& formula);

// Function to add randomly generated clauses to the formula
std::pair<FormulaType, int> addClauses(int numOfVars, int numOfClauses, const FormulaType& formula);

// Function to delete a random number of clauses from the formula
std::pair<FormulaType, int> deleteClauses(int numOfClauses, const FormulaType& formula);

// Function to randomly swap the order of clauses in the formula
FormulaType swapClauses(const FormulaType& formula);

// Function to negate all literals in the formula
FormulaType clauseNegation(const FormulaType& formula);

// Function to add an empty clause to the formula (making it unsatisfiable)
FormulaType addEmptyClause(const FormulaType& formula);

// Function to add a tautological clause to the formula (always true)
FormulaType addTautology(const FormulaType& formula);

// Function to apply a random transformation to the formula
std::tuple<FormulaType, int, std::string> transform(const FormulaType& formula, int numOfClauses, int numOfVars);

// Main function to handle functional transformations on input CNF files
void functionalTransform(const std::string& sutPath, const std::vector<std::string>& inputFiles, unsigned int seed);

#endif // FUNC_HPP
