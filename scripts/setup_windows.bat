@echo off

pushd ..
vendors\premake5\premake5.exe --file=scripts\premake5.lua vs2022

pause