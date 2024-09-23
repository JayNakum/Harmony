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
    "%{prj_src.Inputs}",
    "%{prj_src.Logger}",
    "%{prj_src.Graphics}",
    "%{prj_src.Scene}",

    "%{include_dirs.DirectX12}",
    "%{include_dirs.Sokol}",
    "%{include_dirs.ENTT}",
    "%{include_dirs.PhysX}",
    "%{include_dirs.Box2D}"
	}
  
  links
  {
    -- Links to every project except application
    -- "Engine", -- TODO: Make harmony the ultimate config manager
    "Window",
    "Inputs",
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
    defines { "NDEBUG" }