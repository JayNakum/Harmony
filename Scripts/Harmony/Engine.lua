project "Engine"
  kind "StaticLib"

  files
  {
    "%{prj_src.Harmony.Engine}/**.h",
    "%{prj_src.Harmony.Engine}/**.cpp"
  }

  includedirs
  {
    "%{prj_src.Harmony.Engine}/",
    "%{prj_src.Harmony.Core}/",
    "%{prj_src.Harmony.Graphics}/"
  }

  links
  {
    "Core",
    "Graphics"
  }
