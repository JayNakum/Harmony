import os
import subprocess
import sys

# Runs a shell command and returns its output
def run_command(command: str, exit_on_fail: bool = True) -> None:
	result = subprocess.run(command, capture_output=True, text=True, shell=True)
	
	if result.returncode != 0:
		print(f"[ERROR] Command failed: {command}\n{result.stderr.strip()}")
		if exit_on_fail:
			input()
			sys.exit(1)

	if result.stdout.strip():
		print(f"[INFO] {result.stdout.strip()}")

run_command("Vendors\premake5\premake5.exe --file=Scripts\premake5.lua vs2022")