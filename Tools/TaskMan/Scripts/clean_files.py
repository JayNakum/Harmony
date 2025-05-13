import os

# Cleans the white space on a single line of text
def clean_line(line:str) -> str:
    line = line.rstrip(' \n') + '\n'
    leading_tabs = 0
    while line[leading_tabs] == '\t':
        leading_tabs += 1
    return '    ' * leading_tabs + line[leading_tabs:]

# Cleans the white space in a file
def clean_file(filename:str) -> None:
    try:
        lines = open(filename, 'r', encoding='utf-8').readlines()
        with open(filename, 'w', encoding='utf-8') as outfile:
            for line in lines:
                outfile.write(clean_line(line))
    except OSError:
        print(f'[ERROR] Cleanup for {filename} failed.')

def clean_files(
        working_dir:str = '.', 
        file_extensions:tuple = (), 
        ignore_dir: tuple = ('.git', '.vscode', '__pycache__')
    ) -> None:
    print("[INFO] Cleaning files.")
    for root, dirs, files in os.walk(working_dir):
        # Clean only files with these extensions
        for file in files:
            if file.lower().endswith(file_extensions):
                clean_file(os.path.join(root, file))

        # Do not walk these subdirectories. Remove them from the list
        i = 0
        while i < len(dirs):
            if dirs[i].startswith(ignore_dir):
                del dirs[i]
            else:
                i += 1
    print("[INFO] Files cleaned successfully.")
