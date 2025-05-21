project "Graphics"
  location "%{wks.location}/Harmony/Graphics"
  kind "StaticLib"

  targetdir (target_dir)
  objdir (obj_dir)
  
  files
  {
    "%{prj_src.Harmony.Graphics}/**.h",
    "%{prj_src.Harmony.Graphics}/**.cpp"
  }

  includedirs
  {
    "%{prj_src.Harmony.Graphics}/",

    "%{prj_src.Harmony.Core}/"
  }

  links
  {
    "Core" 
  }

  filter "platforms:Windows"
    includedirs
    {
      "%{prj_src.Platform.Windows}/",

      "$(WindowsSDK_IncludePath)/",
      "$(WindowsSDK_IncludePath)/um",
      "$(WindowsSDK_IncludePath)/shared",
      "$(WindowsSDK_IncludePath)/winrt",
      "$(WindowsSDK_IncludePath)/cppwinrt",

      "%{include_dirs.DirectX12}/"
    }

    links
    {
      "Windows"
    }
