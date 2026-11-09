#!/bin/bash

set -euo pipefail

# Runs the GoogleTest executable on target via Lauterbach T32 and evaluates the XML report.
# Exit codes (from run_t32_tests.py): 0 = all passed, 1 = test failures, 2 = execution/report error.
#
# Environment overrides:
#   LAUTERBACH_T32_EXECUTABLE - path to the T32 ARM debugger executable
#   PYTHON                    - Python 3 interpreter
#   T32_TIMEOUT               - T32 timeout in seconds

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "$SCRIPT_DIR/../.." && pwd)"
cd "$PROJECT_ROOT"

LAUTERBACH_T32_EXECUTABLE="${LAUTERBACH_T32_EXECUTABLE:-/opt/lauterbach/t32/bin/pc_linux64/t32marm}"
PYTHON="${PYTHON:-python3}"
T32_TIMEOUT="${T32_TIMEOUT:-600}"

exec "$PYTHON" "$SCRIPT_DIR/run_t32_tests.py" \
    --t32 "$LAUTERBACH_T32_EXECUTABLE" \
    --script "$PROJECT_ROOT/cpptest.cmm" \
    --xml "$PROJECT_ROOT/.build/gtest_results.xml" \
    --timeout "$T32_TIMEOUT"
