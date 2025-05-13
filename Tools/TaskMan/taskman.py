import sys
sys.dont_write_bytecode = True

from Scripts.clean_files import clean_files
from Scripts.generate_projects import generate_projects
from Scripts.sync import git_sync

# Task dispatch table
TASKS = {
    'clean': {
        'func': clean_files,
        'kwargs': {
            'file_extensions': ('.h', '.cpp', '.c', '.hlsl'),
            'ignore_dir': ('.git', '.vscode', '__pycache__', 'Build', 'Vendors'),
            'working_dir': '.',
        }
    },
    'generate projects': {
        'func': generate_projects,
        'kwargs': {
            'premake_exe': 'Vendors\\premake5\\premake5.exe',
            'premake_file': 'Projects\\premake5.lua',
            'action': 'vs2022',
            'working_dir': '.',
        }
    },
    'sync': {
        'func': git_sync,
        'kwargs': {
            'working_dir': '.',
        }
    }
}

def dispatch(task_name: str, **overrides):
    entry = TASKS.get(task_name)
    if not entry:
        raise ValueError(f'Unknown task: {task_name}')
    # merge defaults with overrides
    kwargs = {**entry['kwargs'], **overrides}
    return entry['func'](**kwargs)

def main():
    # Always run clean first
    dispatch('clean')

    # List tasks and accept choice
    print('Available tasks:')
    for key in TASKS.keys(): print(f'- {key}')

    choice = input('\nEnter task to run: ').strip()
    if choice not in TASKS:
        print(f'[ERROR] Invalid task {choice}.')
        sys.exit(1)

    dispatch(choice)

if __name__ == '__main__':
    main()
