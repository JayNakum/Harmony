from .terminal import run_command

def git_sync(working_dir:str = '.') -> None:
    print('[INFO] Syncing projects.')

    # pull from repository
    print("[INFO] Pulling latest changes from GitHub.")
    run_command("git pull", working_dir=working_dir)

    # detect changes
    print("[INFO] Checking for changes.")
    run_command("git status --porcelain > changelog.txt", working_dir=working_dir)
    run_command("git status", working_dir=working_dir)

    # # stage changes
    # print("[INFO] Staging changes.")
    # run_command("git add .", working_dir=working_dir)

    # # commit changes
    # print("[INFO] Committing changes.")
    # commit_message = input("Enter commit message: ").strip()
    # if not commit_message:
    #     with open("changelog.txt", "r", encoding="utf-8") as f:
    #         commit_message = f.read().strip()
    #         print(commit_message)

    # if commit_message:
    #     run_command(f"git commit -m \"{commit_message}\"", working_dir=working_dir)

    # # push to repository
    # print("[INFO] Pushing changes.")
    # run_command("git push", working_dir=working_dir)

    print("[INFO] Sync completed successfully!")
