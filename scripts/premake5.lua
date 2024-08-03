include "directories.lua"

workspace "Staff"
  location "../build/"
  architecture "x64"
  startproject "SandBox"
  configurations {"Debug", "Release"}
  flags {"MultiProcessorCompile"}

group "Core"
  include "Engine"
  include "Harmony"
  include "Interface"
group "Modules"
  include "Window"
  include "Inputs"
  include "Logger"
  include "Graphics"
  include "Scene"
group "Projects"
  include "../projects/SandBox"
  -- include "../projects/Editor"
  -- include "../projects/BounceMania"