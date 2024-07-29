project "Game"
  kind "StaticLib"
  language "C++"
  cppdialect "C++20"
	staticruntime "on"

  targetdir (target_dir)
  objdir (obj_dir)

  files
  {
    "%{prj_src.Game}/**.h",
    "%{prj_src.Game}/**.cpp"
  }
  
  includedirs
	{
    "%{prj_src.Game}",
    "%{prj_src.Harmony}"
	}

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"