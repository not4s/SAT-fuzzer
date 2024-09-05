#include <gtest/gtest.h>
#include "fuzz/fuzzer.hpp"

// Test if the fuzzer creates the correct directory
TEST(FuzzerTests, DirectoryCreation) {
    // Mock function call
    createDirectory("fuzzed-tests");
    
    // Check if the directory was created
    EXPECT_TRUE(directoryExists("fuzzed-tests"));
    
    // Clean up
    removeDirectory("fuzzed-tests");
}

// Test listing CNF files in a directory
TEST(FuzzerTests, ListCnfFiles) {
    // Assuming you have input files in a test directory
    auto files = listCnfFiles("../inputs/");
    
    // Check if the returned files have the ".cnf" extension
    for (const auto& file : files) {
        EXPECT_EQ(file.substr(file.size() - 4), ".cnf");
    }
}
