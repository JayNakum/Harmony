import subprocess

# Runs a shell command and returns its output
def run_command(
        command:str,
        working_dir:str = None
    ) -> bool:
    
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
        return False
    
    return True
