# AI Coding Agent Instructions for CovGoogleTest

### Directory Structure
```
├── .build/                 # CMake build directory (do not edit)
├── .coverage/              # Coverage data files (.cov)
├── .cpptest/               # C/C++test CT cache directory (do not edit)
├── .devcontainer/          # Configuration files
├── .github/                # Configuration files
├── .vscode/                # Configuration files
├── goo_test.cpp            # GoogleTest unit test cases
├── goo.cpp                 # Main source file
├── goo.h                   # Main header file
├── CMakeLists.txt          # CMake build configuration
└── cpptest-coverage.cmake  # Coverage configuration for C/C++test CT
```

### VSCode Build Tasks
- **CMake: Configure**: Configure the project in the `.build` directory.
- **CMake: Build and Run**: Build the project, execute unit tests, and generate coverage data files in the `.coverage` directory.
- **CMake: Update Coverage**: Update coverage data files in the `.coverage` directory (optional task).

#### Important Notes
- Run **CMake: Configure** before **CMake: Build and Run** to ensure the project is set up correctly.
- Always wait for **CMake: Build and Run** to complete before accessing code coverage information.

### Code Coverage Basics
- Code coverage is measured using Parasoft C/C++test CT.
- Code coverage data files (`.cov`) are stored in the `.coverage` directory.
- Use `cpptestcov` command-line tool to process coverage data and generate reports. For details, use the `search_documentation` MCP tool.

### MCP Tools for Code Coverage
Use these MCP tools for automated coverage analysis:
- `query_line_coverage`: retrieves line coverage details for a file, including coverable, covered, or not-covered lines.
- `query_mcdc_coverage`: retrieves MCDC coverage details for a file, including missing condition values.
- `how_to_suppress_coverage`: provides instructions on suppressing code coverage for specific lines.
- `search_documentation`: searches C/C++test CT documentation for topics like coverage analysis, unit testing, or configuration.
