project "Interface"
  kind "None" -- header only
  language "C++"
  cppdialect "C++17"

  files
  {
    "%{prj_src.Interface}/**.h"
  }
  
  includedirs
	{
    "%{prj_src.Interface}"
	}

  filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
    defines { "NDEBUG" }