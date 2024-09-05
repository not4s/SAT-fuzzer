cmake_minimum_required(VERSION 3.10)
project(SATFuzzer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add Google Test (download and build)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

# Include directories for your source code
include_directories(${PROJECT_SOURCE_DIR}/src)

# Add the main fuzzing executable
add_executable(fuzz-sat
    src/fuzz/fuzzer.cpp
    src/ub/ub.cpp
    src/core/func.cpp
    src/util/util.cpp
)

# Link libraries to the main fuzzing executable (pthread if necessary)
target_link_libraries(fuzz-sat PRIVATE Threads::Threads)

# Define the test executable
add_executable(runTests
    tests/main.cpp
    tests/test_fuzzer.cpp
    tests/test_func.cpp
    tests/test_util.cpp
)

# Link GoogleTest and pthread (for thread safety) to the test executable
target_link_libraries(runTests gtest gtest_main Threads::Threads)

# Enable testing
enable_testing()
add_test(NAME runTests COMMAND runTests)
