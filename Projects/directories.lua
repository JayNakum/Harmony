-- DEFINED PATHS --
action = _ACTION or "null"
target_dir = "%{wks.location}/../Bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
obj_dir = "%{wks.location}/../Obj/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

code_path = "%{wks.location}/../../Code"
vendors_path = "%{wks.location}/../../Vendors"

-- CORE --
prj_src = {}
prj_src["Sandbox"] = "%{code_path}/Sandbox"

prj_src["Harmony"] = {}
prj_src["Harmony"]["Core"] = "%{code_path}/Harmony/Core"
prj_src["Harmony"]["Graphics"] = "%{code_path}/Harmony/Graphics"
prj_src["Harmony"]["Engine"] = "%{code_path}/Harmony/Engine"

prj_src["Platform"] = {}
prj_src["Platform"]["Windows"] = "%{code_path}/Platform/Windows"

-- LIBRARIES --
include_dirs = {}
include_dirs["DirectX12"] = "%{vendors_path}/DirectX-Headers/include"
