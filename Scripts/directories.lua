-- DEFINED PATHS --
target_dir = "%{wks.location}/Bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
obj_dir = "%{wks.location}/Bin/Obj/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

-- CORE --
prj_src = {}
prj_src["Sandbox"] = "%{wks.location}/../Source/Sandbox"

prj_src["Harmony"] = {}
prj_src["Harmony"]["Core"] = "%{wks.location}/../Source/Harmony/Core"
prj_src["Harmony"]["Graphics"] = "%{wks.location}/../Source/Harmony/Graphics"

prj_src["Platform"] = {}
prj_src["Platform"]["Windows"] = "%{wks.location}/../Source/Platform/Windows"

-- LIBRARIES --
DIRECTX_SDK = os.getenv("DXSDK_DIR")
include_dirs = {}
include_dirs["DirectX"] = "%{DIRECTX_SDK}/Include"
include_dirs["DirectX12"] = "%{wks.location}/../Vendors/DirectX-Headers/include"

lib_dirs = {}
lib_dirs["DirectX"] = "%{DIRECTX_SDK}/Lib"
