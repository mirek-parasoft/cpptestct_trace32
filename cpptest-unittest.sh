#!/bin/bash

set -euo pipefail

# == CMake project ==

cmake -DCPPTEST_COVERAGE=ON -DCMAKE_TOOLCHAIN_FILE=$PWD/arm-none-eabi-toolchain.cmake -S . -B .build

# Build
cmake --build .build 

# Clean up old results
rm -rf .build/cpptest-coverage/*/*.clog .build/gtest-report

# Run tests on target and generate coverage reports
./cpptest-t32-execution.sh || true
make -C .build cpptestcov-compute cpptestcov-suppress cpptestcov-report