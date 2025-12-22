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
    "-Werror",
    
    "-Wno-c++98-compat-pedantic",
    "-Wno-gnu-anonymous-struct",
    "-Wno-nested-anon-types",
    "-Wno-covered-switch-default",
    "-Wno-language-extension-token",
    "-Wno-missing-field-initializers",

    "-Wno-global-constructors",
    "-Wno-unsafe-buffer-usage"
  }

  configurations { "Debug", "Profile", "Release" }

  filter "system:Windows"
    architecture "x64"
    defines { "HWINDOWS" }
    
  filter "configurations:Debug"
    runtime "Debug"
    symbols "On"
    defines { "HDEBUG" }

  filter "configurations:Profile"
    runtime "Release"
    optimize "On"
    defines { "HPROFILE" }

  filter "configurations:Release"
    runtime "Release"
    optimize "On"
    defines { "HRELEASE" }

  filter {}

group "Platform"
  include "Platform/Windows"
group "Vendors"
  include "Vendors/ImGUI"
group "Harmony"
  include "Harmony/Core"
  include "Harmony/Maths"
  include "Harmony/Graphics"
  include "Harmony/Engine"
group ""
  include "Sandbox"
