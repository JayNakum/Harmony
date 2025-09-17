#include "DebugUI.h"

#include "dx12/Swapchain.h"

#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"

namespace engine
{
    std::vector<std::function<void()>> DebugUI::msRenderCallbacks;

    DebugUI::DescriptorHeapWithAllocator::DescriptorHeapWithAllocator(graphics::Device& device)
        : mAllocationSize(device.getDescriptorHandleIncrementSize(graphics::DescriptorHeapType::ShaderViews)),
        mAllocator(MEMORY_BUDGET),
        mDescriptorHeap(
            device,
            {
                "DebugUIDescriptorHeap_ShaderViewsGPU",
                graphics::DescriptorHeapType::ShaderViews,
                MEMORY_BUDGET / mAllocationSize,
                graphics::DescriptorHeapFlags::GPUVisible
            }
        ) {}

    graphics::DescriptorHandle DebugUI::DescriptorHeapWithAllocator::allocate()
    {
        const size_t allocationOffset = mAllocator.allocate(mAllocationSize);
        return mDescriptorHeap.getHeapStart() + (INT)allocationOffset;
    }

    void DebugUI::matrix(const char* label, mat4f& matrix)
    {
        if (ImGui::TreeNode(label))
        {
            ImGui::BeginTable(label, 4, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_NoHostExtendX);
            for (int i = 0; i < 4; ++i)
            {
                ImGui::TableNextRow();
                for (int j = 0; j < 4; ++j)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text("%.3f", matrix[i][j]);
                }
            }
            ImGui::EndTable();
            ImGui::TreePop();
        }
    }

    void DebugUI::matrixInput(const char* label, mat4f& matrix)
    {
        if (ImGui::TreeNode(label))
        {
            ImGui::BeginTable(label, 4, ImGuiTableFlags_Borders);
            for (int i = 0; i < 4; ++i)
            {
                ImGui::TableNextRow();
                for (int j = 0; j < 4; ++j)
                {
                    ImGui::TableNextColumn();
                    ImGui::PushID(i * 4 + j);
                    ImGui::InputFloat("##val", &matrix[i][j]);
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
            ImGui::TreePop();
        }
    }

    DebugUI::DebugUI(const HWND windowHandle, graphics::Device& device, graphics::CommandQueue& graphicsQueue)
        : mShaderViewsDescriptorHeapGPU(device)
    {
        msRenderCallbacks.clear();

        // Make process DPI aware and obtain main monitor scale
        ImGui_ImplWin32_EnableDpiAwareness();
        float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
        io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
        io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(windowHandle);

        io.UserData = &mShaderViewsDescriptorHeapGPU;

        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = device.get();
        init_info.CommandQueue = graphicsQueue.get();
        init_info.NumFramesInFlight = graphics::Swapchain::BACK_BUFFER_COUNT;
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
        // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
        // (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
        init_info.SrvDescriptorHeap = mShaderViewsDescriptorHeapGPU.getHeap().get();
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* init_info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
            {
                DebugUI::DescriptorHeapWithAllocator* descriptorHeap = static_cast<DebugUI::DescriptorHeapWithAllocator*>(ImGui::GetIO().UserData);
                graphics::DescriptorHandle newHandle = descriptorHeap->allocate();

                out_cpu_handle->ptr = newHandle.getCPUPtr();
                out_gpu_handle->ptr = newHandle.getGPUPtr();
            };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE) {};
        ImGui_ImplDX12_Init(&init_info);
    }

    void DebugUI::render(graphics::GraphicContext& context)
    {
        context.bindDescriptorHeap(mShaderViewsDescriptorHeapGPU.getHeap());

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        const char* dockspace_name = "DebugUI_MainDockspace";
        ImGuiID dockspace_id = ImGui::GetID(dockspace_name);

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 100, main_viewport->WorkPos.y + 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1280, 800), ImGuiCond_FirstUseEver);

        ImGuiWindowFlags host_flags = ImGuiWindowFlags_None;
        ImGui::Begin("Harmony", nullptr, host_flags);
        ImGui::DockSpace(dockspace_id);

        // Execute all registered debug UI callbacks
        for (const auto& callback : msRenderCallbacks)
        {
            ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);

            callback();
        }

        ImGui::End();

        // Rendering
        ImGui::Render();

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), context.commandList().operator->());

        ImGuiIO& io = ImGui::GetIO();
        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    DebugUI::~DebugUI()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        msRenderCallbacks.clear();
    }
}
