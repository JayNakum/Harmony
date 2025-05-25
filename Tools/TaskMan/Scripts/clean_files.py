import os
import shutil

# Cleans the white space on a single line of text
def clean_line(line: str) -> str:
    line = line.rstrip(' \n') + '\n'
    leading_tabs = 0
    while leading_tabs < len(line) and line[leading_tabs] == '\t':
        leading_tabs += 1
    return '    ' * leading_tabs + line[leading_tabs:]

# Cleans the white space in a file
def clean_file(filename: str) -> bool:
    try:
        lines = open(filename, 'r', encoding='utf-8').readlines()
        with open(filename, 'w', encoding='utf-8') as outfile:
            for line in lines:
                outfile.write(clean_line(line))
        return True
    except OSError:
        print(f'[ERROR] Cleanup for {filename} failed.')
        return False

def clean_files(
        file_extensions: tuple, 
        ignore_dir: tuple,
        working_dir: str) -> bool:
    
    print("[INFO] Cleaning files.")
    okay = True

    for root, dirs, files in os.walk(working_dir):
        for file in files:
            if file.lower().endswith(file_extensions):
                filepath = os.path.join(root, file)
                okay &= clean_file(filepath)

        # Do not walk these subdirectories. Remove them from the list
        i = 0
        while i < len(dirs):
            if dirs[i].startswith(ignore_dir):
                del dirs[i]
            else:
                i += 1

    if okay:
        print("[INFO] Files cleaned successfully.")

    return okay

def delete_folder(folder_path:str, working_dir:str) -> bool:
    full_path = os.path.join(working_dir, folder_path)
    if os.path.exists(full_path):
        print(f"[INFO] Deleting {full_path}")
        shutil.rmtree(full_path)
        print(f"[INFO] {full_path} deleted successfully.")
    else:
        print(f"[INFO] {full_path} directory not found.")
    return True