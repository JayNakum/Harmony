project "Core"
  kind "StaticLib"

  files
  {
    "%{prj_src.Harmony.Core}/**.*"
  }

  includedirs
  {
    "%{prj_src.Harmony.Core}/"
  }
