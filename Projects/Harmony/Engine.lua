project "Engine"
  location "%{wks.location}/Engine/"
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
