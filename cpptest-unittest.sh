#!/bin/bash

set -euo pipefail

# == CMake project ==

cmake -DCPPTEST_COVERAGE=ON -DCMAKE_TOOLCHAIN_FILE=$PWD/arm-none-eabi-toolchain.cmake -S . -B .build

# Build
cmake --build .build 

# Clean up old results
cd .build && rm -rf cpptest-coverage/*/*.clog && rm -rf gtest-report 

# Run tests and generate coverage reports
ctest || true && make cpptestcov-compute cpptestcov-suppress cpptestcov-report