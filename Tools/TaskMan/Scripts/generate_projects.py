from .terminal import run_command

def generate_projects(
        working_dir:str = '.',
        premake_exe:str = 'Vendors/premake5/premake5.exe',
        premake_file:str = 'Projects/premake5.lua',
        action:str = 'vs2022') -> None:
    print('[INFO] Generating projects.')
    command = f"{premake_exe} --file={premake_file} {action}"
    run_command(command, working_dir=working_dir)
    print('[INFO] Projects generated successfully.')
