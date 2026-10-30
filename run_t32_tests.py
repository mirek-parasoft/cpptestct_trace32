#!/usr/bin/env python3
"""Run GoogleTest on target via Lauterbach T32 and evaluate the XML report."""

import argparse
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--t32", required=True, help="Path to t32marm executable")
    parser.add_argument("--script", required=True, help="PRACTICE script (.cmm) to execute")
    parser.add_argument("--xml", required=True, help="GoogleTest XML report written by the script")
    parser.add_argument("--timeout", type=float, default=600, help="T32 timeout in seconds")
    return parser.parse_args()


def run_t32(t32, script, timeout):
    cmd = [t32, "--t32-screen=off", "-s", script]
    print("Running:", " ".join(cmd), flush=True)
    try:
        result = subprocess.run(cmd, timeout=timeout)
    except FileNotFoundError:
        print(f"ERROR: T32 executable not found: {t32}", file=sys.stderr)
        return None
    except subprocess.TimeoutExpired:
        print(f"ERROR: T32 did not finish within {timeout} s", file=sys.stderr)
        return None
    return result.returncode


def evaluate(xml_path):
    try:
        root = ET.parse(xml_path).getroot()
    except FileNotFoundError:
        print(f"ERROR: test report not found: {xml_path}", file=sys.stderr)
        return 2
    except ET.ParseError as e:
        print(f"ERROR: invalid test report {xml_path}: {e}", file=sys.stderr)
        return 2

    passed, failed, skipped, disabled = [], [], [], []
    for suite in root.iter("testsuite"):
        for case in suite.iter("testcase"):
            name = f"{case.get('classname', suite.get('name'))}.{case.get('name')}"
            failures = case.findall("failure") + case.findall("error")
            if failures:
                failed.append((name, failures))
            elif case.find("skipped") is not None or case.get("result") == "skipped":
                skipped.append(name)
            elif case.get("status") == "notrun":
                disabled.append(name)
            else:
                passed.append(name)

    for name in passed:
        print(f"[       OK ] {name}")
    for name in skipped:
        print(f"[  SKIPPED ] {name}")
    for name in disabled:
        print(f"[ DISABLED ] {name}")
    for name, failures in failed:
        print(f"[  FAILED  ] {name}")
        for f in failures:
            print("    " + (f.text or f.get("message", "")).strip().replace("\n", "\n    "))

    total = len(passed) + len(failed) + len(skipped) + len(disabled)
    print()
    print(f"Total: {total}, Passed: {len(passed)}, Failed: {len(failed)}, "
          f"Skipped: {len(skipped)}, Disabled: {len(disabled)}")

    if total == 0:
        print("ERROR: no test cases found in report", file=sys.stderr)
        return 2
    return 1 if failed else 0


def main():
    args = parse_args()
    xml_path = Path(args.xml)
    # Prevent a stale report from a previous run being evaluated.
    xml_path.unlink(missing_ok=True)

    rc = run_t32(args.t32, args.script, args.timeout)
    if rc is None:
        return 2
    if rc != 0:
        print(f"WARNING: T32 exited with code {rc}", file=sys.stderr)

    result = evaluate(xml_path)
    if result == 0 and rc != 0:
        return 2
    return result


if __name__ == "__main__":
    sys.exit(main())
