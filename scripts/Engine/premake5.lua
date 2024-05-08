project "Engine"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.Engine}/**.h",
    "%{prj_src.Engine}/**.cpp"
  }
  
  includedirs
	{
    "%{prj_src.Engine}",
    "%{prj_src.Harmony}",
    "%{prj_src.Interface}",

    "%{include_dirs.Sokol}"
	}
  
  links
  {
    "Harmony"
  }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"