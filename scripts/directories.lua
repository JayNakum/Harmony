-- DEFINED PATHS --
target_dir = "%{wks.location}/Bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
obj_dir = "%{wks.location}/Bin/Obj/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

-- CORE --
prj_src = {}
prj_src["Sandbox"] = "%{wks.location}/../Source/Sandbox"

prj_src["Harmony"] = "%{wks.location}/../Source/Harmony"
prj_src["Core"] = "%{wks.location}/../Source/Harmony/Core"
prj_src["Engine"] = "%{wks.location}/../Source/Harmony/Engine"

-- LIBRARIES --
DIRECTX_SDK = os.getenv("DXSDK_DIR")
include_dirs = {}
include_dirs["DirectX"] = "%{DIRECTX_SDK}/Include"
include_dirs["DirectX12"] = "%{wks.location}/../Vendors/DirectX-Headers/include"

lib_dirs = {}
lib_dirs["DirectX"] = "%{DIRECTX_SDK}/Lib"
