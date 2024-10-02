project "Logger"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.Logger}/**.h",
    "%{prj_src.Logger}/**.cpp"
  }
  
  includedirs
	{
    "%{prj_src.Logger}",
    "%{prj_src.Interface}",
    "%{prj_src.Harmony}",
    
    "%{include_dirs.Sokol}"
	}

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
    defines { "NDEBUG" }