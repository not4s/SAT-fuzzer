#ifndef UB_HPP
#define UB_HPP

#include <string>
#include <vector>
#include <regex>

std::string createInput();
std::string createGarbage();
std::string makeCnfFile(const std::string& cnf, int fileNumber);
void saveInput(const std::string& outputPath, const std::string& content, int index, const std::string& prefix);
std::string runSUT(const std::string& inputFilename, const std::string& sutPath);
int checkUb(const std::string& sutOutput);
void generateUB(const std::string& sutPath, const std::string& inputsPath, unsigned int seed, int maxIterations);

#endif // UB_HPP