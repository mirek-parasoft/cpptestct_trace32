---
name: cpptest-fix-one-violation
description: "Use when: fixing a single, already-known static analysis violation in C or C++ code reported by Parasoft C/C++test"
---

# C/C++test Fix One Violation Skill

This skill fixes a single Parasoft C/C++test static analysis violation that the user has already identified and described in their prompt. The analysis is not run upfront — the violation details are taken directly from the user's prompt. Verification of the fix is only performed when explicitly requested by the user.

## When to Use This Skill

Use this skill when:
- The user describes a **single, concrete violation instance** that is identified or deducible by rule ID, message, file, and line number
- The violation is already known — no discovery phase is needed

Do NOT use this skill when:
- The user asks to fix "all violations of rule X" or multiple violations — use `cpptest-fix-all-violations` instead

## Assets

This skill uses the following external resources and tools:
| Type | Name | Description | Failure Handling |
| --- | --- | --- | --- |
| Script | `cpptest-analyze.sh` | Builds the project and runs Parasoft C/C++test analysis. This is **the only way** to run the analysis; using other commands or tools may lead to incorrect results. | Report error and exit |
| MCP Tool | `get_violations_from_report_file` | Fetches violations from a Parasoft C/C++test report file. | Report error and exit |
| MCP Tool | `get_rule_documentation` | Fetches rule documentation from Parasoft C/C++test. | Report warning and proceed |

## Workflow

Overview of the workflow:
- Extract violation details from the user's prompt
- Get rule documentation
- Apply the fix
- Verify the fix *(only if explicitly requested by the user)*

Be sure to follow the workflow steps in order. Steps should be executed sequentially, without user intervention, until the final output is generated.

### Step 1: Extract Violation Details from the Prompt

Parse the user's prompt to identify the following fields. All fields are optional but at least a **rule ID** or a clear description of the violation must be present to proceed. The **file and line** are also important — use them to locate the exact violation when available:

| Field | Description |
| --- | --- |
| Rule ID | The Parasoft C/C++test rule identifier (e.g., `MISRA2008-5_0_3`) |
| File | Source file path where the violation occurs |
| Line | Line number of the violation |
| Message | The violation message text |

If the prompt does not contain enough information to identify the violation, ask the user to clarify before proceeding.

### Step 2: Get Rule Documentation

**Always** use the `get_rule_documentation` MCP tool to fetch documentation for the rule identified in Step 1. This documentation may include the rule description, rationale, examples of compliant and non-compliant code, and guidance on how to fix violations.

If the rule ID is unknown, skip this step and rely on the violation message and code context.

### Step 3: Apply the Fix

Analyze the violation details (message, file, line), the source code at the relevant location, and the rule documentation. Apply the **smallest change possible** to resolve the violation. Do not fix violations of other rules or make unrelated changes.

### Step 4: Verify the Fix *(optional)*

**Skip this step unless the user explicitly asks for verification** (e.g., "verify", "check", "make sure it's fixed").

If verification is requested, execute the following script in the root of the project:

```bash
./cpptest-analyze.sh
```

This is **the only** way of building the project and running the analysis — **do not** use any other commands or tools.

Use the `get_violations_from_report_file` MCP tool to collect violations from `reports/report.xml`. Check whether the violation identified in Step 1 is still present:

- **If the violation is gone**: the fix is successful — report the result to the user.
- **If the violation remains**: analyze the output, revise the fix, and re-verify. Repeat up to **3 attempts** total. If all attempts fail, declare the violation unfixable and report the result to the user without reverting the last attempted fix.

If verification was **not** requested, display the following message to the user at the end:

> **Note:** Verification was not requested and has been skipped. The fix has been applied but not confirmed. Run the analysis manually to verify the violation is resolved.
