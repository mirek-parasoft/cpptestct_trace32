#!/bin/bash

set -euo pipefail

# Runs the GoogleTest executable on target via Lauterbach T32 and evaluates the XML report.
# Exit codes (from run_t32_tests.py): 0 = all passed, 1 = test failures, 2 = execution/report error.
#
# Environment overrides:
#   LAUTERBACH_T32_EXECUTABLE - path to the T32 ARM debugger executable
#   PYTHON                    - Python 3 interpreter
#   T32_TIMEOUT               - T32 timeout in seconds

# cpptest.cmm uses paths relative to the project root
cd "$(dirname "$0")"

LAUTERBACH_T32_EXECUTABLE="${LAUTERBACH_T32_EXECUTABLE:-/opt/lauterbach/t32/bin/pc_linux64/t32marm}"
PYTHON="${PYTHON:-python3}"
T32_TIMEOUT="${T32_TIMEOUT:-600}"

exec "$PYTHON" run_t32_tests.py \
    --t32 "$LAUTERBACH_T32_EXECUTABLE" \
    --script cpptest.cmm \
    --xml .build/gtest_results.xml \
    --timeout "$T32_TIMEOUT"
