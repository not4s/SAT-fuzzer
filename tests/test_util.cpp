#include <gtest/gtest.h>
#include <fstream>
#include "../src/util/util.hpp"

TEST(UtilTests, DirectoryCreationAndRemoval) {
    std::string testDir = "test-dir";
    createDirectory(testDir);

    // Test if directory exists
    EXPECT_EQ(system(("test -d " + testDir).c_str()), 0);

    // Clean up the directory
    removeDirectory(testDir);
    EXPECT_NE(system(("test -d " + testDir).c_str()), 0);
}

TEST(UtilTests, SaveInputTest) {
    std::string content = "p cnf 3 2\n1 -2 0\n2 3 0";
    std::string outputPath = "test_test_1.cnf";
    
    // Save the input
    saveInput(".", content, 1, "test");
    
    // Check if the file was created
    std::ifstream inFile(outputPath);
    EXPECT_TRUE(inFile.is_open());
    
    // Optionally, verify the content
    std::string fileContent((std::istreambuf_iterator<char>(inFile)),
                             std::istreambuf_iterator<char>());
    EXPECT_EQ(fileContent, content);
    
    inFile.close();
    
    // Clean up
    remove(outputPath.c_str());
}
