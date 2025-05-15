from .terminal import run_command

def generate_projects(
        working_dir: str = '.',
        premake_exe: str = 'Vendors/premake5/premake5.exe',
        premake_file: str = 'Projects/premake5.lua',
        action: str = 'vs2022') -> bool:
    
    print('[INFO] Generating projects.')
    command = f"{premake_exe} --file={premake_file} {action}"
    okay = run_command(command, working_dir=working_dir)

    if okay:
        print('[INFO] Projects generated successfully.')
    return okay