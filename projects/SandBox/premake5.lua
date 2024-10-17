project "SandBox"
  kind "WindowedApp"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.SandBox}/**.h",
    "%{prj_src.SandBox}/**.cpp"
  }
  
  includedirs
	{
    "%{prj_src.SandBox}",
    
    "%{prj_src.Engine}",
    "%{prj_src.Harmony}",
    "%{prj_src.Interface}",

    "%{include_dirs.GLM}",
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