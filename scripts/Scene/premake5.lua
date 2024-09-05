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
    "%{include_dirs.PhysX}",
    "%{include_dirs.Box2D}"
	}

  links
  {
    "PhysX_static_64",
    "PhysXCommon_static_64",
    "PhysXCooking_static_64",
    "PhysXExtensions_static_64",
    "PhysXFoundation_static_64",
    "PhysXPvdSDK_static_64",
    -- "PhysXVehicle_static_64",
    -- "PhysXVehicle2_static_64",
    "PVDRuntime_64",
    
    "box2d"
  }

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
    libdirs 
    {
      "%{lib_dirs.PhysX}/debug",
      "%{lib_dirs.Box2D}/Debug"
    }

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
    libdirs 
    {
      "%{lib_dirs.PhysX}/release",
      "%{lib_dirs.Box2D}/Release"
    }
