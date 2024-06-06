include "directories.lua"

workspace "Staff"
  location "../build/"
  architecture "x64"
  startproject "Engine"
  configurations {"Debug", "Release"}
  flags {"MultiProcessorCompile"}

include "Engine"
include "Harmony"
include "Interface"
group "Interfaces"
  include "Window"
  include "Logger"
  include "Graphics"
  include "Scene"
