include "directories.lua"

workspace "Harmony"
  location "../Build/%{action}"
  startproject "Sandbox"
  flags { "MultiProcessorCompile" }

  language "C++"
  toolset "clang"
  cppdialect "C++20"
  staticruntime "on"
  buildoptions {
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wno-c++98-compat-pedantic"
  }

  platforms { "Windows" }
    configurations { "Debug", "Profile", "Release" }

  filter "platforms:Windows"
    architecture "x64"
    defines { "WINDOWS" }
    
  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    defines { "DEBUG" }
    -- buildoptions { "-Werror" }

  filter "configurations:Profile"
    runtime "Release"
    optimize "On"
    defines { "PROFILE" }

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
    defines { "RELEASE" }

group "Platform"
  include "Platform/Windows"
group "Vendors"
  include "Vendors/ImGUI"
group "Harmony"
  include "Harmony/Core"
  include "Harmony/Maths"
  include "Harmony/Graphics"
  include "Harmony/Engine"
include "Sandbox"
