project "Scene"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.Scene}/**.h",
    "%{prj_src.Scene}/**.cpp"
  }
  
  includedirs
	{
    "%{prj_src.Scene}",
    "%{prj_src.Interface}",
    "%{prj_src.Harmony}",

    "%{include_dirs.ENTT}",
    "%{include_dirs.PhysX}"
	}

  links
  {
    "PhysXDevice64.dll",
    "PhysXGpu_64.dll"
  }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
    libdirs 
    {
      "%{lib_dirs.PhysX}/debug"
    }
    links
    {
      "freeglutd.dll",
    }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
    libdirs 
    {
      "%{lib_dirs.PhysX}/release"
    }
    links
    {
      "freeglut.dll",
    }
