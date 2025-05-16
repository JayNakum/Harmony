import sys
sys.dont_write_bytecode = True

import time
from plyer import notification

from config import TASKS

def dispatch(task_name:str, **overrides):
    entry = TASKS.get(task_name)
    # merge defaults with overrides
    kwargs = {**entry['kwargs'], **overrides}
    return entry['func'](**kwargs)

def main():
    # Always run clean first
    dispatch('clean')

    print('\nAvailable tasks:')
    for key in TASKS:
        print(f'- {key}')

    raw_input_str = input('Enter tasks to run (comma-separated): ')
    # comma separated tasks
    choices = [s.strip() for s in raw_input_str.strip().lower().split(',') if s.strip()]
    task_statuses = {}

    start_time_total = time.perf_counter()
    for task_name in choices:
        print()  # Ensure separation before task logs

        if task_name not in TASKS:
            print(f'[ERROR] Invalid task: "{task_name}"')
            task_statuses[task_name] = False
        else:
            print(f"[INFO] Running task: {task_name}")
            start_time = time.perf_counter()
            success = dispatch(task_name)
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
