import sys
sys.dont_write_bytecode = True

from plyer import notification
from tqdm import tqdm

from Scripts.clean_files import clean_files
from Scripts.sync import sync
from Scripts.generate_projects import generate_projects
from Scripts.sync import submit

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
    'sync': {
        'func': sync,
        'kwargs': {
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
    'submit': {
        'func': submit,
        'kwargs': {
            'working_dir': '.',
        }
    }
}

def dispatch(task_name: str, **overrides):
    entry = TASKS.get(task_name)
    # merge defaults with overrides
    kwargs = {**entry['kwargs'], **overrides}
    return entry['func'](**kwargs)

def main():
    # Always run clean first
    dispatch('clean')

    # List tasks and accept choice
    print('\nAvailable tasks:')
    for key in TASKS.keys(): print(f'- {key}')

    raw_choices = input('Enter tasks to run (comma-separated): ')
    choices = [s.strip() for s in raw_choices.strip().lower().split(',') if s.strip()]

    task_statuses = {}  # { task_name: success_bool }

    for choice in tqdm(choices, desc="Executing tasks", unit="task"):
        if not choice:
            continue
        if choice not in TASKS:
            print(f'[ERROR] Invalid task: "{choice}"')
            task_statuses[choice] = False
        else:
            success = dispatch(choice)
            task_statuses[choice] = success

    # Notification message
    if all(task_statuses.values()):
        title = 'TaskMan Tasks Completed Successfully'
    else:
        title = 'TaskMan Tasks Completed with Errors'
    
    message_lines = [
        f"({'success' if status else 'error'}) {task}"
        for task, status in task_statuses.items()
    ]

    notification.notify(
        title=title,
        message='\n'.join(message_lines),
        app_name='TaskMan'
    )


if __name__ == '__main__':
    main()
