import clean_files as cleaner

# cleanup files
print("[INFO] Preparing files.")
cleaner.clean_files()

from terminal import run_command

# generate projects
print("[INFO] Generating projects.")
run_command(r"Vendors\premake5\premake5.exe --file=Scripts/premake5.lua vs2022")
