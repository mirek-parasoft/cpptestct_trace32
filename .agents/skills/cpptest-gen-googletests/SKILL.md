---
name: cpptest-gen-googletests
description: "Use when: generating Google Test cases for C or C++ code to improve MC/DC and line coverage"
---

# C/C++test Generate Google Tests Skill

This skill generates Google Test cases for C or C++ code. Code changes can be optionally committed to the repository.

## When to Use This Skill

Use this skill when:
- User wants to generate Google Test cases for C or C++ code to improve MC/DC and line coverage

## Assets

This skill uses the following external resources and tools:
| Type | Name | Description | Failure Handling |
| --- | --- | --- | --- |
| Script | `helper/scripts/cpptest-unittest.sh` | Builds the project, runs available Google Test cases and collects code coverage. This is **the only way** to run unit tests; using other commands or tools may lead to incorrect results. | Report error and exit |
| MCP Tool | `query_mcdc_coverage` | Fetches MC/DC coverage results from the executed Google Test cases, together with information about the missing conditional values required to achieve full coverage. | Report error and exit |
| MCP Tool | `query_line_coverage` | Fetches coverable, covered, not-covered, or suppressed lines for a source file. | Report error and exit |

## Workflow

Overview of the workflow:
- Read configuration parameters
- Run unit tests and generate coverage results
- Analyze MC/DC and line coverage results and generate new GoogleTest test cases
- Execute all test cases including newly generated ones and check both coverage results
- Repeat workflow until both MC/DC and line coverage reach 100%
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

### Step 2: Run unit tests and generate coverage results

Execute the following script in the root of the project:
```bash
./helper/scripts/cpptest-unittest.sh
```

This is **the only** way of building the project, executing unit tests and generating coverage reports - **do not** use any other commands or tools.

### Step 3: Analyze MC/DC and line coverage results and generate new GoogleTest test cases

For each source file under test, call `query_mcdc_coverage` with the absolute `.coverage` directory and source-file paths to read its MC/DC results and missing-condition hints. Then call `query_line_coverage` for that same file with the same absolute paths and `query_type: "notcovered"` to find uncovered lines. Call the tools sequentially, one file at a time. **Do not** inspect report files manually or use other tools to determine coverage; use the MCP results.

If all tested files have 100% MC/DC and `query_line_coverage` reports no uncovered coverable lines, finish the workflow **immediately**.

Read the "Tested condition values" and "Missing condition values" information from the MC/DC tool output. Use the "Missing condition values" to create tests for uncovered conditional behavior, and use the line-coverage output to target uncovered executable lines. Avoid generating tests for conditions or lines that are already covered. If the line-coverage output includes suppressed lines, do not treat them as actionable uncovered lines.

Check the last commit message for the information about implemented requirements. If the commit message includes requirement association in the form of `Requirement: <requirement_id>`, use this information to annotate the generated test cases accordingly. Test cases annotation should be done by injecting the following function call at the beginning of each generated test case: `RecordProperty("req", "<requirement_id>");`. The assumption is that uncovered code is related to the implemented requirement(s). Do not analyze this information manually, just assume it is correct. All generated test cases should include this annotation. If no requirement association is provided in the commit message, skip adding annotations to generated test cases.

### Step 4: Execute all test cases including newly generated ones

Execute the following script in the root of the project:
```bash
./helper/scripts/cpptest-unittest.sh
```

This is **the only** way of building the project, executing unit tests and generating coverage reports - **do not** use any other commands or tools.


### Step 5: Repeat workflow until both coverage goals are achieved

After executing generated tests, use `query_mcdc_coverage` and `query_line_coverage` again to check all tested files. Repeat from Step 3 until both MC/DC and line coverage are 100%, or the number of attempts reaches **iteration_limit** (from Step 1). Do not stop just because one coverage type has reached 100%. If either goal cannot be achieved, analyze the reason. For justified cases like unreachable code or platform dependent code behaviour apply suppression. Use `how_to_suppress_coverage` to learn about suppressions. In all other cases of uncoverable code just report the final results and the remaining missing conditions or uncovered lines.

### Step 6: Commit all generated GoogleTest unit test cases (Conditional)

This step should only be performed if any test cases were generated AND if **commit_tests** (from Step 1) is `true`. If **commit_tests** is `false`, skip this step and report to the output that the commit was skipped for the current rule.

Commit changes to the repository using `git commit` command. Be sure to follow the commit message guidelines below to ensure consistency and clarity in the commit history. **Do not** use any other committer e-mail, name, or commit message format, as this may lead to confusion and inefficient workflow:
- **Always** use 'autofix@demo' as committer e-mail.
- **Always** use 'Autofix' as committer name.
- Use a verbose commit message that includes the final MC/DC and line coverage results.
- The first line of the commit message must be exactly "New auto generated test cases: <number of generated test cases>", followed by a blank line. The subsequent lines should provide detailed information about the final MC/DC and line coverage results and any additional context relevant to the generated tests.
