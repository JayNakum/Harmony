import sys
sys.dont_write_bytecode = True

import time
from plyer import notification

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
            'commit': None,
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

def dispatch(task_name:str, **overrides):
    entry = TASKS.get(task_name)
    # merge defaults with overrides
    kwargs = {**entry['kwargs'], **overrides}
    return entry['func'](**kwargs)

def parse_task_input(task_str:str):
    # colon indicates arguments
    if ':' in task_str:
        task_name, args_str = task_str.split(':', 1)
        kwargs = {}
        # pipe separated arguments
        for arg in args_str.split('|'):
            if '=' in arg:
                key, value = arg.split('=', 1)
                kwargs[key.strip()] = value.strip()
        return task_name.strip(), kwargs
    else:
        return task_str.strip(), {}

def main():
    # Always run clean first
    dispatch('clean')

    print('\nAvailable tasks:')
    for key in TASKS:
        print(f'- {key}')

    raw_input_str = input('Enter tasks to run (semicolon-separated): ')
    

    # semicolon separated tasks
    raw_tasks = [s.strip() for s in raw_input_str.strip().split(';') if s.strip()]
    
    parsed_tasks = [parse_task_input(task_str) for task_str in raw_tasks]
    task_statuses = {}

    start_time_total = time.perf_counter()
    for task_name, overrides in parsed_tasks:
        print()  # Ensure separation before task logs

        if task_name not in TASKS:
            print(f'[ERROR] Invalid task: "{task_name}"')
            task_statuses[task_name] = False
        else:
            print(f"[INFO] Running task: {task_name}")
            start_time = time.perf_counter()
            success = dispatch(task_name, **overrides)
            elapsed = time.perf_counter() - start_time
            print(f'Done({elapsed:.3f}).')
            task_statuses[task_name] = success
    elapsed_total = time.perf_counter() - start_time_total

    # Notification
    if all(task_statuses.values()):
        title = f'TaskMan Tasks Completed Successfully ({elapsed_total:.3f})'
    else:
        title = f'TaskMan Tasks Completed with Errors ({elapsed_total:.3f})'

    message_lines = [
        f"({'success' if status else 'error'}) {task}"
        for task, status in task_statuses.items()
    ]
    
    print(f'\n{title}')

    notification.notify(
        title=title,
        message='\n'.join(message_lines),
        app_name='TaskMan'
    )


if __name__ == '__main__':
    main()
