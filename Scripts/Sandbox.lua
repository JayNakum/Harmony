project "Sandbox"
  kind "WindowedApp"

  files
  {
    "%{prj_src.Sandbox}/**.*"
  }

  includedirs
  {
    "%{prj_src.Sandbox}/",
    "%{prj_src.Harmony.Core}/",
    "%{prj_src.Harmony.Graphics}/"
  }

  links
  {
    "Core",
    "Graphics"
  }

  filter "platforms:Windows"
    includedirs
    {
      "%{prj_src.Platform.Windows}/"
    }

    links
    {
      "Windows"
    }
