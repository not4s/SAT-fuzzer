#!/usr/bin/env bash
set -x # Display every executed instruction
set -e # Fail script as soon as instruction fails

# Assuming your source files are in the same directory as this script
SOURCE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SOURCE_DIR/build"

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Compile the fuzzer using g++
g++ -std=c++17 "$SOURCE_DIR/fuzzer.cpp" "$SOURCE_DIR/ub.cpp" "$SOURCE_DIR/func.cpp" "$SOURCE_DIR/util.cpp" -o "fuzz-sat"

exit 0
