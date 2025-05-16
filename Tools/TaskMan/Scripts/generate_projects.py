from .terminal import run_command

def generate_projects(
        premake_exe: str,
        premake_file: str,
        action: str,
        working_dir: str) -> bool:
    
    print('[INFO] Generating projects.')
    command = f"{premake_exe} --file={premake_file} {action}"
    okay = run_command(command, working_dir=working_dir)

    if okay:
        print('[INFO] Projects generated successfully.')
    return okay