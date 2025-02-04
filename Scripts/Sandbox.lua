project "Sandbox"
  kind "WindowedApp"

  files
  {
    "%{prj_src.Sandbox}/**.*",
    "%{prj_src.Harmony}/**.*"
  }

  includedirs
    {
    "%{prj_src.Sandbox}/",
    "%{prj_src.Harmony}/",
    "%{include_dirs.DirectX}/",
    "%{include_dirs.DirectX12}/"
    }

  libdirs
  {
    "%{lib_dirs.DirectX}/"
  }

  links
  {
    "d3d12.lib",
    "dxgi.lib",
    "d3dcompiler.lib",
    "dxguid.lib"
  }
