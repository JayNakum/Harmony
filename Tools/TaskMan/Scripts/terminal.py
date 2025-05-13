import subprocess
import sys

# Runs a shell command and returns its output
def run_command(
        command:str,
        exit_on_fail:bool = True,
        working_dir:str = None
    ) -> None:
    
    result = subprocess.run(
        command,
        capture_output=True,
        text=True,
        shell=True,
        cwd=working_dir
    )
    
    if result.stdout.strip():
        print(result.stdout.strip())

    if result.stderr.strip():
        print(result.stderr.strip())

    # If the command fails
    if result.returncode != 0:
        print(f"[ERROR] Command failed: {command}")
        if exit_on_fail:
            input("Press Enter to exit...")
            sys.exit(1)
