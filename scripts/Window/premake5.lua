project "Window"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"
  characterset ("MBCS")

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.Window}/**.h",
    "%{prj_src.Window}/**.cpp"
  }
  
  includedirs
	{
    "%{prj_src.Window}",
    "%{prj_src.Interface}",
    "%{prj_src.Harmony}"
	}

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
    defines { "NDEBUG" }