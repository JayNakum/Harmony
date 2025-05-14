-- DEFINED PATHS --
target_dir = "%{wks.location}/../Bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
obj_dir = "%{wks.location}/Bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

-- CORE --
prj_src = {}
prj_src["Sandbox"] = "%{wks.location}/../Code/Sandbox"

prj_src["Harmony"] = {}
prj_src["Harmony"]["Core"] = "%{wks.location}/../Code/Harmony/Core"
prj_src["Harmony"]["Graphics"] = "%{wks.location}/../Code/Harmony/Graphics"
prj_src["Harmony"]["Engine"] = "%{wks.location}/../Code/Harmony/Engine"

prj_src["Platform"] = {}
prj_src["Platform"]["Windows"] = "%{wks.location}/../Code/Platform/Windows"

-- LIBRARIES --
include_dirs = {}
include_dirs["DirectX12"] = "%{wks.location}/../Vendors/DirectX-Headers/include"
