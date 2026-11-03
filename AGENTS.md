# Instructions

- Always use the `cpptest-fix-all-violations` skill when the user wants to fix multiple or all violations — including when they ask to fix all violations of a specific rule or severity.
- Always use the `cpptest-fix-one-violation` skill only when the user wants to fix a single, concrete violation instance they have already pinpointed — identified or deducible by rule ID, message, file, and line number. Do NOT use this skill when the user asks to fix all violations of a rule or all violations matching some criteria.