#!/usr/bin/env python3
import os
import subprocess
import sys

# Configuration
BUILD_DIR = os.path.join(os.path.dirname(__file__), "../build")
EXAMPLES_DIR = os.path.join(os.path.dirname(__file__), "../examples")
COMPILER_BIN = os.path.join(BUILD_DIR, "loc")

def run_test(file_path):
    """Runs a single test file. Returns True if passed, False otherwise."""
    print(f"Running {os.path.basename(file_path)}...", end=" ")
    
    try:
        # Run compiler with the file argument
        result = subprocess.run(
            [COMPILER_BIN, file_path],
            capture_output=True,
            text=True,
            timeout=5
        )
        
        # Determine expected outcome based on filename
        # Files starting with 'fail' are expected to fail (exit code != 0)
        is_negative_test = os.path.basename(file_path).startswith("fail")
        
        if is_negative_test:
            if result.returncode != 0:
                print("PASSED (Expected Failure)")
                return True
            else:
                print("FAILED (Unexpected Success)")
                return False
        else:
            if result.returncode == 0:
                print("PASSED")
                return True
            else:
                print(f"FAILED (Exit Code {result.returncode})")
                print("stderr:", result.stderr)
                return False

    except Exception as e:
        print(f"ERROR: {e}")
        return False

def main():
    if not os.path.exists(COMPILER_BIN):
        print(f"Error: Compiler not found at {COMPILER_BIN}")
        print("Please build the project first.")
        sys.exit(1)

    files = [f for f in os.listdir(EXAMPLES_DIR) if f.endswith(".loc")]
    files.sort()
    
    passed = 0
    total = len(files)
    
    print(f"Found {total} test cases in {EXAMPLES_DIR}\n")
    
    for f in files:
        if run_test(os.path.join(EXAMPLES_DIR, f)):
            passed += 1
            
    print("-" * 40)
    print(f"Summary: {passed}/{total} tests passed.")
    
    if passed == total:
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()
