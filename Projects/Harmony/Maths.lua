project "Maths"
  location "%{wks.location}/Harmony/Maths"
  kind "None"

  files
  {
    "%{prj_src.Harmony.Maths}/**.h",
    "%{prj_src.Harmony.Maths}/**.cpp"
  }

  includedirs
  {
    "%{prj_src.Harmony.Maths}/",
    
    "%{prj_src.Harmony.Core}/"
  }
