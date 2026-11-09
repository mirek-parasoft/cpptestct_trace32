#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

# == CMake project ==

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -S . -B .build

# Build
cmake --build .build

# Analyze
cpptestcli -quiet -compiler gcc_14-aarch64 -config "./MISRA C++ 2023.properties" -module . -exclude */syscalls.cpp -exclude */comm-plugin/* -exclude */google*/* -exclude *_test.cpp -report .report -input .build/compile_commands.json