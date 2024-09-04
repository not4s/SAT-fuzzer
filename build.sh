#!/usr/bin/env bash
set -x # Display every executed instruction
set -e # Fail script as soon as instruction fails

# Set the build directory
SOURCE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SOURCE_DIR/build"

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Compile the fuzzer using g++
g++ -std=c++17 \
    "$SOURCE_DIR/src/fuzz/fuzzer.cpp" \
    "$SOURCE_DIR/src/ub/ub.cpp" \
    "$SOURCE_DIR/src/core/func.cpp" \
    "$SOURCE_DIR/src/util/util.cpp" \
    -I"$SOURCE_DIR/src" \
    -o "$BUILD_DIR/fuzz-sat"

exit 0
