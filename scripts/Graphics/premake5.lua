project "Graphics"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.Graphics}/**.h",
    "%{prj_src.Graphics}/**.cpp",
    "%{prj_src.Graphics}/**.c"
  }
  
  includedirs
	{
    "%{prj_src.Graphics}",
    "%{prj_src.Interface}",
    "%{prj_src.Harmony}",
    
    "%{include_dirs.Sokol}"
	}
  
  -- graphics APIs
  links
  {
    "%{lib_dirs.DirectX}/d3d11.lib",
    "%{lib_dirs.DirectX}/d3dx11.lib",
    "%{lib_dirs.DirectX}/d3dx11d.lib"
  }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"