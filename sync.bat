@echo off

if not exist ".git" (
    echo [ERROR] This is not a Git repository. Ensure the script is placed correctly.
    pause
    exit /b 1
)

echo [INFO] Pulling latest changes from GitHub...
git pull
if errorlevel 1 (
    echo [ERROR] Failed to pull changes from GitHub.
    pause
    exit /b 1
)

echo [INFO] Checking for changes...
git status --porcelain > changelog.txt
for /f %%A in (changelog.txt) do (
    goto :changes_found
)

echo [INFO] No changes detected. Skipping commit and push.
goto :no_changes_found
exit /b 0

:changes_found
git status
echo [INFO] Adding changes to the staging area...
git add .

echo [INFO] Committing changes...
set /p COMMIT_MSG="Enter commit message: "
git commit -m "%COMMIT_MSG%"
if errorlevel 1 (
    echo [ERROR] Commit failed. Please check the status of your repository.
    pause
    exit /b 1
)

echo [INFO] Pushing changes to GitHub...
git push
if errorlevel 1 (
    echo [ERROR] Failed to push changes to GitHub.
    pause
    exit /b 1
)

:no_changes_found
echo [INFO] Sync completed successfully!
pause
exit /b 0
