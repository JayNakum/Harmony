from Scripts.clean_files import clean_files, delete_folder
from Scripts.code_changes import sync, submit
from Scripts.generate_projects import generate_projects

TASKS = {
    'clean': {
        'func': clean_files,
        'kwargs': {
            'file_extensions': ('.h', '.cpp', '.c', '.hlsl'),
            'ignore_dir': ('.git', '.vscode', '__pycache__', 'Build', 'Tools', 'Vendors'),
            'working_dir': '.',
        }
    },
    'clean build': {
        'func': delete_folder,
        'kwargs': {
            'folder_path': 'Build',
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
