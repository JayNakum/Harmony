project "Sandbox"
  kind "WindowedApp"

  files
  {
    "%{prj_src.Sandbox}/**.h",
    "%{prj_src.Sandbox}/**.cpp"
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
