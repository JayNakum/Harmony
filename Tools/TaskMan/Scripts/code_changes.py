from .terminal import run_command

def status(working_dir: str) -> bool:
    print("[INFO] Checking for changes.")
    okay = run_command("git status --porcelain > changelog.txt", working_dir=working_dir)
    okay &= run_command("git status", working_dir=working_dir)
    return okay

def sync(commit:str, working_dir: str) -> bool:
    print('[INFO] Syncing projects.')
    print("[INFO] Pulling latest changes from GitHub.")
    
    okay = True

    if commit:
        print(f"[INFO] Fetching latest and checking out commit {commit}.")
        okay &= run_command("git fetch", working_dir=working_dir)
        okay &= run_command(f"git checkout {commit}", working_dir=working_dir)
    else:
        print("[INFO] Pulling latest changes.")
        okay &= run_command("git pull", working_dir=working_dir)
    
    okay &= run_command("git submodule update --init --recursive", working_dir=working_dir)
    okay &= status(working_dir)

    if okay:
        print("[INFO] Sync completed successfully!")
        
    return okay

def submit(working_dir: str) -> bool:
    okay = status(working_dir)
    
    print("[INFO] Staging changes.")
    okay &= run_command("git add .", working_dir=working_dir)

    print("[INFO] Committing changes.")
    commit_message = input("Enter commit message: ").strip()
    if not commit_message:
        with open("changelog.txt", "r", encoding="utf-8") as f:
            commit_message = f.read().strip()
            print(commit_message)

    if commit_message:
        okay &= run_command(f"git commit -m \"{commit_message}\"", working_dir=working_dir)

    print("[INFO] Pushing changes.")
    okay &= run_command("git push", working_dir=working_dir)

    return okay
