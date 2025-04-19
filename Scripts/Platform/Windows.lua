project "Windows"
  kind "StaticLib"

  files
  {
    "%{prj_src.Platform.Windows}/**.h",
    "%{prj_src.Platform.Windows}/**.cpp"
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
    "dxguid.lib",

    "d3dcompiler.lib",

    "dinput8.lib",
    "xinput9_1_0.lib"
  }
