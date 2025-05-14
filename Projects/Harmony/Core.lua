project "Core"
  location "%{wks.location}/Core/"
  kind "StaticLib"

  files
  {
    "%{prj_src.Harmony.Core}/**.h",
    "%{prj_src.Harmony.Core}/**.cpp"
  }

  includedirs
  {
    "%{prj_src.Harmony.Core}/"
  }
