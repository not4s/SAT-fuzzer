#ifndef FUNC_HPP
#define FUNC_HPP

#include <vector>
#include <string>
#include <random>
#include <utility>
#include <tuple>
#include <fstream>

// Type definition for representing a formula (list of clauses)
using FormulaType = std::vector<std::vector<std::string>>;

// Constants for different SAT/UNSAT scenarios
extern const std::string FUNC_TESTS;
extern const std::string SAT_UNKNOWN__UNSAT_UNKNOWN;
extern const std::string SAT_UNKNOWN__UNSAT_UNSAT;
extern const std::string SAT_SAT__UNSAT_UNKOWN;
extern const std::string SAT_SAT__UNSAT_UNSAT;
extern const std::string SAT_UNSAT__UNSAT_UNSAT;

// Function prototypes
FormulaType swapLiterals(const FormulaType& formula);
std::pair<FormulaType, int> addClauses(int numOfVars, int numOfClauses, const FormulaType& formula);
std::pair<FormulaType, int> deleteClauses(int numOfClauses, const FormulaType& formula);
FormulaType swapClauses(const FormulaType& formula);
FormulaType clauseNegation(const FormulaType& formula);
FormulaType addEmptyClause(const FormulaType& formula);
FormulaType addTautology(const FormulaType& formula);
std::tuple<FormulaType, int, std::string> transform(const FormulaType& formula, int numOfClauses, int numOfVars);
void functionalTransform(const std::string& sutPath, const std::vector<std::string>& inputFiles, unsigned int seed);

#endif // FUNC_HPP
