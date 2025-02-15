project "Graphics"
  kind "StaticLib"

  files
  {
    "%{prj_src.Harmony.Graphics}/**.*"
  }

  includedirs
  {
    "%{prj_src.Harmony.Graphics}/",
    "%{prj_src.Harmony.Core}/"
  }

  filter "platforms:Windows"
    includedirs
    {
      "%{prj_src.Platform.Windows}/",
      "%{include_dirs.DirectX}/",
      "%{include_dirs.DirectX12}/"
    }

    links
    {
      "Windows"
    }
