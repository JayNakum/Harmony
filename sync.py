import os
import subprocess
import sys

# Ensure script is running inside a Git repository
if not os.path.exists(".git"):
    print("[ERROR] This is not a Git repository. Ensure the script is placed in a repository folder.")
    sys.exit(1)

from terminal import run_command
import clean_files as cleaner

cleaner.clean_files()

# pull from repository
print("[INFO] Pulling latest changes from GitHub.")
run_command("git pull")

# detect changes
print("[INFO] Checking for changes.")
run_command("git status --porcelain > changelog.txt")
run_command("git status")

# stage changes
print("[INFO] Staging changes.")
run_command("git add .")

# commit changes
print("[INFO] Committing changes.")
commit_message = input("Enter commit message: ").strip()
if not commit_message:
    with open("changelog.txt", "r", encoding="utf-8") as f:
        commit_message = f.read().strip()
        print(commit_message)

if commit_message:
    run_command(f"git commit -m \"{commit_message}\"")

# push to repository
print("[INFO] Pushing changes.")
run_command("git push")

print("[INFO] Sync completed successfully!")
