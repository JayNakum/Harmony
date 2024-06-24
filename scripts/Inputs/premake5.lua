project "Inputs"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.Inputs}/**.h",
    "%{prj_src.Inputs}/**.cpp"
  }
  
  includedirs
	{
    "%{prj_src.Inputs}",
    "%{prj_src.Interface}",
    "%{prj_src.Harmony}"
	}

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"