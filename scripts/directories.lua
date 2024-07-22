target_dir = "%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
obj_dir = "%{wks.location}/bin/obj/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

prj_src = {}
prj_src["Engine"] = "%{wks.location}/../source/Engine"
prj_src["Interface"] = "%{wks.location}/../source/Interface"
prj_src["Harmony"] = "%{wks.location}/../source/Harmony"
prj_src["Window"] = "%{wks.location}/../source/Window"
prj_src["Inputs"] = "%{wks.location}/../source/Inputs"
prj_src["Logger"] = "%{wks.location}/../source/Logger"
prj_src["Graphics"] = "%{wks.location}/../source/Graphics"
prj_src["Scene"] = "%{wks.location}/../source/Scene"

DIRECTX_SDK = os.getenv("DXSDK_DIR")

include_dirs = {}
include_dirs["DirectX"] = "%{DIRECTX_SDK}/Include"
include_dirs["DirectX12"] = "%{wks.location}/../vendors/DirectX-Headers/include"
include_dirs["Sokol"] = "%{wks.location}/../vendors/sokol"
include_dirs["GLM"] = "%{wks.location}/../vendors/glm/glm"
include_dirs["ENTT"] = "%{wks.location}/../vendors/entt/src/entt"

lib_dirs = {}
lib_dirs["DirectX"] = "%{DIRECTX_SDK}/Lib"

-- function addLibrariesInFolder(folder)
--   local files = os.matchfiles(folder .. "/*.lib")
--   for _, file in ipairs(files) do
--       local libname = path.getbasename(file)
--       libdirs { path.getdirectory(file) }
--       links { libname }
--   end
-- end
