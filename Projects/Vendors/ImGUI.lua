project "ImGUI"
  location "%{wks.location}/Vendors/ImGUI"
  kind "StaticLib"
  systemversion "latest"

  targetdir ("%{target_dir}/%{prj.name}")
  objdir ("%{obj_dir}/%{prj.name}")
  
  warnings "Off"
  buildoptions
  {
    "-Wno-everything",
    "-Wno-error"
  }

  files
  {
    "%{prj_src.Vendors.imgui}/imconfig.h",
    "%{prj_src.Vendors.imgui}/imgui.h",
    "%{prj_src.Vendors.imgui}/imgui.cpp",
    "%{prj_src.Vendors.imgui}/imgui_demo.cpp",
    "%{prj_src.Vendors.imgui}/imgui_draw.cpp",
    "%{prj_src.Vendors.imgui}/imgui_internal.h",
    "%{prj_src.Vendors.imgui}/imgui_tables.cpp",
    "%{prj_src.Vendors.imgui}/imgui_widgets.cpp",
    "%{prj_src.Vendors.imgui}/imstb_rectpack.h",
    "%{prj_src.Vendors.imgui}/imstb_textedit.h",
    "%{prj_src.Vendors.imgui}/imstb_truetype.h",
  }

  includedirs
  {
  	"%{prj_src.Vendors.imgui}/",
  	"%{prj_src.Vendors.imgui}/backends",
  }

  filter "system:Windows"
    files
    {
      "%{prj_src.Vendors.imgui}/backends/imgui_impl_win32.h",
      "%{prj_src.Vendors.imgui}/backends/imgui_impl_win32.cpp",
      "%{prj_src.Vendors.imgui}/backends/imgui_impl_dx12.h",
      "%{prj_src.Vendors.imgui}/backends/imgui_impl_dx12.cpp"
    }

  filter {}
