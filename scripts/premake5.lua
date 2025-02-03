include "directories.lua"

workspace "Harmony"
  location "../Build/"
  startproject "Sandbox"
  flags { "MultiProcessorCompile" }
  
  platforms { "Windows" }
    configurations { "Debug", "Release" }
  
  filter "platforms:Windows"
    architecture "x64"
    defines { "WINDOWS" }
  
  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    defines { "DEBUG" }
  
	filter "configurations:Release"
    runtime "Release"
    optimize "On"
    defines { "RELEASE" }
  
  language "C++"
  cppdialect "C++20"
	staticruntime "on"
  targetdir (target_dir)
  objdir (obj_dir)

include "Sandbox"