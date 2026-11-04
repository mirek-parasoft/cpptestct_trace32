---
name: cpptest-gen-googletests
description: "Use when: generating Google Test cases for C or C++ code to ensure high MC/DC code coverage"
---

# C/C++test Generate Google Tests Skill

This skill generates Google Test cases for C or C++ code. Code changes can be optionally committed to the repository.

## When to Use This Skill

Use this skill when:
- User wants to generate Google Test cases for C or C++ code to ensure high MC/DC code coverage

## Assets

This skill uses the following external resources and tools:
| Type | Name | Description | Failure Handling |
| --- | --- | --- | --- |
| Script | `cpptest-unittest.sh` | Builds the project, runs available Google Test cases and collects code coverage. This is **the only way** to run unit tests; using other commands or tools may lead to incorrect results. | Report error and exit |
| MCP Tool | `query_mcdc_coverage` | Fetches MC/DC coverage results from the executed Google Test cases, together with information about the missing condtional values required to achieve full coverage. | Report error and exit |

## Workflow

Overview of the workflow:
- Read configuration parameters
- Run unit tests and generate MC/DC coverage results
- Analyze MC/DC coverage results and generate new GoogleTest test cases
- Execute all test cases including newly generated and check the MC/DC coverage results for completeness
- Repeat workflow until 100% MC/DC coverage is achieved
- Commit all generated GoogleTest unit test cases (Conditional)


Be sure to follow the workflow steps in order. Steps should be executed sequentially, without user intervention, until the final output is generated.

### Step 1: Read Configuration Parameters

Read the following environment variables to configure the workflow:

```bash
echo $CPPTEST_COMMIT_TESTS
echo $CPPTEST_ITERATION_LIMIT
```

| Variable | Valid Values | Default | Description |
| --- | --- | --- | --- |
| `CPPTEST_COMMIT_TESTS` | `true`, `1` | *(not set)* | If set to `true` or `1`, commit generated test cases. Any other value is ignored. |
| `CPPTEST_ITERATION_LIMIT` | positive integer | `3` | Maximum number of generate-and-execute attempts before declaring 100% coverage is not achievable. If the value is not a positive integer, use the default of `3`. |

Store these values as **commit_tests** (boolean), and **iteration_limit** (integer) for use in later steps. The user's explicit request to commit (if any) also sets **commit_tests** to `true`. The user's explicit request for an iteration limit (if any) overrides **iteration_limit**.

### Step 2: Run unit tests and generate MC/DC coverage results

Execute the following script in the root of the project:
```bash
./cpptest-unittest.sh
```

This is **the only** way of building the project, executing unit tests and generating coverage reports - **do not** use any other commands or tools.

### Step 3: Analyze MC/DC coverage results and generate new GoogleTest test cases

Use `query_mcdc_coverage` MCP tool to read MC/DC coverage results and hints regarding uncovered conditional values from `.coverage` directory generated during coverage reports generation. **Do not** analyze the report files manually or use any other tools to get MC/DC coverage information, as this may lead to incorrect results and inefficient workflow.

If there is 100% MC/DC code coverage reported for all tested files, finish the workflow **immediately**.

Read carefully the "Tested condition values" and "Missing condition values" information from MCP tool output. Use information about "Missing condition values" to generate new GoogleTest test cases that specifically target the uncovered conditional values. Avoid generating test cases for conditional values that are already covered.

### Step 4: Execute all test cases including newly generated ones

Execute the following script in the root of the project:
```bash
./cpptest-unittest.sh
```

This is **the only** way of building the project, executing unit tests and generating coverage reports - **do not** use any other commands or tools.


### Step 5: Repeat workflow until 100% MC/DC coverage is achieved

Use `query_mcdc_coverage` MCP tool to read MC/DC coverage results after executing generated tests. If the coverage is not 100% repeat from Step 3 until full coverage is achieved, or the number of attempts reaches **iteration_limit** (from Step 1)

### Step 6: Commit all generated GoogleTest unit test cases (Conditional)

This step should only be performed if any test cases were generated AND if **commit_tests** (from Step 1) is `true`. If **commit_tests** is `false`, skip this step and report to the output that the commit was skipped for the current rule.