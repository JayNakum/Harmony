project "Windows"
  location ".%{wks.location}/Platform/Windows"
  kind "StaticLib"
  systemversion "latest"

  targetdir ("%{target_dir}/%{prj.name}")
  objdir ("%{obj_dir}/%{prj.name}")
  
  files
  {
    "%{prj_src.Platform.Windows}/**.h",
    "%{prj_src.Platform.Windows}/**.cpp"
  }

  includedirs
  {
    "%{prj_src.Platform.Windows}/",
    "%{prj_src.Harmony.Core}/",

    "$(WindowsSDK_IncludePath)/",
    "$(WindowsSDK_IncludePath)/um",
    "$(WindowsSDK_IncludePath)/shared",
    "$(WindowsSDK_IncludePath)/winrt",
    "$(WindowsSDK_IncludePath)/cppwinrt",

    "%{include_dirs.DirectX12}/"
  }

  libdirs
  {
    "$(WindowsSDK_LibraryPath)/um/x64"
  }

  links
  {
    "Core",
    
    "d3d12.lib",
    "dxgi.lib",
    "dxguid.lib",

    "d3dcompiler.lib",

    "dinput8.lib",
    "xinput9_1_0.lib"
  }
