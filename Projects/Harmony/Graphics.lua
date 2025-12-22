project "Graphics"
  location "%{wks.location}/Harmony/Graphics"
  kind "StaticLib"

  targetdir ("%{target_dir}/%{prj.name}")
  objdir ("%{obj_dir}/%{prj.name}")
  
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

  filter "system:Windows"
    includedirs
    {
      "%{prj_src.Platform.Windows}/"
    }
    
    externalincludedirs
    {
      "$(WindowsSDK_IncludePath)/",
      "$(WindowsSDK_IncludePath)/um",
      "$(WindowsSDK_IncludePath)/shared",
      "$(WindowsSDK_IncludePath)/winrt",
      "$(WindowsSDK_IncludePath)/cppwinrt",
  
      "%{include_dirs.DirectX12}/",
      "%{include_dirs.imgui}/"
    }

    links
    {
      "Windows",
      "ImGUI"
    }
  
  filter {}
