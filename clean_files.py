import os
import sys

# Cleans the white space on a single line of text
def clean_line(line):
    line = line.rstrip(' \n') + '\n'
    leading_tabs = 0
    while line[leading_tabs] == '\t':
        leading_tabs += 1
    return '    ' * leading_tabs + line[leading_tabs:]

# Cleans the white space in a file
def clean_file(filename):
    try:
        lines = open(filename, 'r').readlines()
        with open(filename, 'w') as outfile:
            for line in lines:
                outfile.write(clean_line(line))
    except OSError:
        print(f'[ERROR] Cleanup for {filename} failed.')

def clean_files(start_path = '.'):
    if len(sys.argv) > 1:
        start_path = sys.argv[1]
    for root, dirs, files in os.walk(start_path):
        # Clean only files with these extensions
        for file in files:
            if file.lower().endswith(('.h', '.cpp', '.c', '.hlsl', '.py', '.lua')):
                clean_file(os.path.join(root, file))

        # Do not walk these subdirectories. Remove them from the list
        i = 0
        while i < len(dirs):
            if dirs[i].startswith(('.git', '.vscode', '__pycache__', 'Build', 'Vendors')):
                del dirs[i]
            else:
                i += 1
