@echo off

Vendors\premake5\premake5.exe --file=Scripts\premake5.lua vs2022

if errorlevel 1 (
    echo "Error: Failed to generate projects."
    pause
    exit /b 1
)

pause
exit /b 0
