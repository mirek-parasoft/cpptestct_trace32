#!/bin/bash

set -euo pipefail

# This script should run AI Agent for C/C++test Static Analysis with AI Autofix.
# It is intended to be executed by 'cpptest-autofix-github.yml' but can also be run manually.

# == Codex ==

# Register C/C++test MCP server - assuming C/C++test is installed in '/opt/parasoft/cpptest' location, adjust as needed
codex mcp add cpptest-std-mcp -- /opt/parasoft/cpptest/integration/mcp/cpptestmcp

# Execute the prompt with Codex - be sure to adjust sandbox permissions as needed for your prompt
codex exec -s danger-full-access --config allow_login_shell=false "$(cat "$(dirname "$0")/cpptest-agent-prompt.md")"

# == Copilot ==

# Register C/C++test MCP server - assuming C/C++test is installed in '/opt/parasoft/cpptest' location, adjust as needed
# copilot mcp get cpptest-std-mcp >/dev/null 2>&1 || copilot mcp add cpptest-std-mcp -- /opt/parasoft/cpptest/integration/mcp/cpptestmcp

# Execute the prompt with Copilot - be sure to adjust sandbox permissions as needed for your prompt
# copilot --allow-all --no-ask-user -s -p "$(cat "$(dirname "$0")/cpptest-agent-prompt.md")"
