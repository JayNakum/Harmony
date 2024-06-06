project "Harmony"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.Harmony}/**.h",
    "%{prj_src.Harmony}/**.cpp"
  }
  
  includedirs
	{
    "%{prj_src.Harmony}",
    "%{prj_src.Interface}",
    "%{prj_src.Window}",
    "%{prj_src.Logger}",
    "%{prj_src.Graphics}",
    "%{prj_src.Scene}",

    "%{include_dirs.Sokol}",
    "%{include_dirs.ENTT}"
	}
  
  links
  {
    -- Links to every project except application
    "Window",
    "Logger",
    "Graphics",
    "Scene"
  }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"