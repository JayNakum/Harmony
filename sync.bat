@echo off

if not exist ".git" (
    echo [ERROR] This is not a Git repository. Ensure the script is placed correctly.
    pause
    exit /b 1
)

git pull

git status --porcelain > changelog.txt
git status

git add .

set /p COMMIT_MSG="Enter commit message: "
git commit -m "%COMMIT_MSG%"

git push

pause
exit /b 0
