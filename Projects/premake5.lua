include "directories.lua"

workspace "Harmony"
  location "../Build/%{action}"
  startproject "Sandbox"
  flags { "MultiProcessorCompile" }

  platforms { "Windows" }
    configurations { "Debug", "Profile", "Release" }

  filter "platforms:Windows"
    architecture "x64"
    defines { "WINDOWS" }

  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    defines { "DEBUG" }

  filter "configurations:Profile"
    runtime "Release"
    optimize "On"
    defines { "PROFILE" }

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
    defines { "RELEASE" }

  language "C++"
  cppdialect "C++20"
  staticruntime "on"

group "Platform"
  include "Platform/Windows"
group "Harmony"
  include "Harmony/Core"
  include "Harmony/Graphics"
  include "Harmony/Engine"
include "Sandbox"
