import sys
sys.dont_write_bytecode = True

from Scripts.clean_files import clean_files
from Scripts.generate_projects import generate_projects
from Scripts.sync import git_sync

# === CONFIGURATION ===
DEFAULT_PATHS = {
    'clean': '.',
    'generate': '.',
    'sync': '.',
}
PREMAKE_CONFIG = {
    'exe':    'Vendors\\premake5\\premake5.exe',
    'script': 'Projects\\premake5.lua',
    'action': 'vs2022',
}
# File extensions and directories to ignore when cleaning
CLEAN_CONFIG = {
    'file_extensions': ('.h', '.cpp', '.c', '.hlsl'),
    'ignore_dir': ('.git', '.vscode', '__pycache__', 'Build', 'Vendors'),
}
# =================

# Task dispatch table
TASKS = {
    'clean': {
        'func': clean_files,
        'kwargs': {
            'working_dir': DEFAULT_PATHS['clean'],
            'file_extensions': CLEAN_CONFIG['file_extensions'],
            'ignore_dir': CLEAN_CONFIG['ignore_dir'],
        }
    },
    'generate': {
        'func': generate_projects,
        'kwargs': {
            'working_dir': DEFAULT_PATHS['generate'],
            'premake_exe': PREMAKE_CONFIG['exe'],
            'premake_file': PREMAKE_CONFIG['script'],
            'vs_version': PREMAKE_CONFIG['action'],
        }
    },
    'sync': {
        'func': git_sync,
        'kwargs': {
            'working_dir': DEFAULT_PATHS['sync'],
        }
    }
}

def dispatch(task_name: str, **overrides):
    entry = TASKS.get(task_name)
    if not entry:
        raise ValueError(f"Unknown task: {task_name}")
    # merge defaults with overrides
    kwargs = {**entry['kwargs'], **overrides}
    return entry['func'](**kwargs)

def main():
    # Always run clean first
    dispatch('clean')

    # List tasks and accept choice
    print("Available tasks:", ', '.join(TASKS.keys()))
    choice = input("Enter task to run: ").strip()
    if choice not in TASKS:
        print(f"Invalid task '{choice}'. Exiting.")
        sys.exit(1)

    dispatch(choice)

if __name__ == "__main__":
    main()
