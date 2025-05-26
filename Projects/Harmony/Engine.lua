project "Engine"
  location "%{wks.location}/Harmony/Engine"
  kind "StaticLib"

  targetdir ("%{target_dir}/%{prj.name}")
  objdir ("%{obj_dir}/%{prj.name}")

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
