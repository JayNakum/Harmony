project "Windows"
  kind "StaticLib"

  files
  {
    "%{prj_src.Platform.Windows}/**.*"
  }

  includedirs
  {
    "%{prj_src.Platform.Windows}/",
    "%{prj_src.Harmony.Core}/",

    "%{include_dirs.DirectX}/",
    "%{include_dirs.DirectX12}/"
  }

  libdirs
  {
    "%{lib_dirs.DirectX}/"
  }

  links
  {
    "Core",

    "d3d12.lib",
    "dxgi.lib",
    "d3dcompiler.lib",
    "dxguid.lib"
  }
