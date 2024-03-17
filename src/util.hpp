#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>

bool directoryExists(const std::string& path);
void removeDirectory(const std::string& path);
void createDirectory(const std::string& path);
void saveInput(const std::string& outputPath, const std::string& content, int id, const std::string& funcType);
void saveError(const std::string& outputPath, const std::string& content, int id, const std::string& funcType);
std::string runSUT(const std::string& inputFilename, const std::string& sutPath);
int checkUb(const std::string& sutOutput);
void evictFilesIfNeeded();

#endif // UTIL_HPP
