project "Core"
  location "%{wks.location}/Harmony/Core"
  kind "StaticLib"

  targetdir (target_dir)
  objdir (obj_dir)
  
  files
  {
    "%{prj_src.Harmony.Core}/**.h",
    "%{prj_src.Harmony.Core}/**.cpp"
  }

  includedirs
  {
    "%{prj_src.Harmony.Core}/"
  }
