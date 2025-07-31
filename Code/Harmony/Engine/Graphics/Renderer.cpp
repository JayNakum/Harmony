#include "Renderer.h"

#include "ShaderInput.h"

namespace engine
{
    Renderer::Renderer(const HWND windowHandle)
        : mDevice(),
        mGraphicsQueue(mDevice, {"Renderer_Graphics_Queue", graphics::CommandListType::Direct}),
        mComputeQueue(mDevice, {"Renderer_Compute_Queue", graphics::CommandListType::Compute}),
        mResourceManager(mDevice),
        mCommandContextPool(),
        mSwapchain(
            mDevice, mGraphicsQueue,
            {
                "Renderer_Swapchain",
                windowHandle
            },
            {
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU)
            }
        ),
        mClientWidth(0), mClientHeight(0),
        mWaterPass(mDevice, mResourceManager),
        mPresentPass(mDevice, mResourceManager),
        mNoisePass(mDevice, mResourceManager)
    {
        {
            graphics::GraphicContext& initGraphicsContext = aquireGraphicContext("Init_Graphics_Context");

            mWaterPass.copyDataToGPU(initGraphicsContext, mResourceManager);
            mPresentPass.copyDataToGPU(initGraphicsContext, mResourceManager);

            initGraphicsContext.finish(mGraphicsQueue.incrementFence());

            graphics::CommandList batchedGraphicsCommandLists[1] = { initGraphicsContext.commandList() };
            uint64_t graphicsFence = mGraphicsQueue.executeCommandLists(batchedGraphicsCommandLists);
            mGraphicsQueue.waitForFence(graphicsFence);

            discardContext(initGraphicsContext);
            mWaterPass.freeGPUUploadData(mResourceManager);
            mPresentPass.freeGPUUploadData(mResourceManager);
        }
        {
            graphics::ComputeContext& initComputeContext = aquireComputeContext("Init_Compute_Context");

            mNoisePass.copyDataToGPU(initComputeContext, mResourceManager);

            initComputeContext.finish(mComputeQueue.incrementFence());

            graphics::CommandList batchedComputeCommandLists[1] = { initComputeContext.commandList() };
            uint64_t computeFence = mComputeQueue.executeCommandLists(batchedComputeCommandLists);
            mComputeQueue.waitForFence(computeFence);

            discardContext(initComputeContext);
            mNoisePass.freeGPUUploadData(mResourceManager);
        }

        mResourceManager.reset(graphics::HeapWithAllocator::ResourceCategory::UploadData);
    }

    void Renderer::resize(const unsigned int width, const unsigned int height)
    {
        mClientWidth = width;
        mClientHeight = height;

        mComputeQueue.waitForFence(mComputeQueue.incrementFence());
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());
        mSwapchain.resize(
            mDevice, width, height,
            {
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU)
            }
        );

        mWaterPass.resize(width, height, mDevice, mResourceManager);
        mPresentPass.resize(width, height, mDevice, mResourceManager, mSwapchain.isHDRSupported());

        mNoisePass.resize(width, height, mDevice, mResourceManager);
    }

    void Renderer::update(const double timeStep)
    {
        mWaterPass.update(timeStep);
        mPresentPass.update(timeStep);

        mNoisePass.update(timeStep);
    }

    void Renderer::render(const FrameConstants frame)
    {
        const unsigned int frameI = mSwapchain.getCurrentBackBufferIndex();

        // NOISE PASS
        graphics::ComputeContext& contextNoise = aquireComputeContext("Noise_Compute_Context");

        mNoisePass.getInputOutputs(contextNoise, mResourceManager);
        mNoisePass.dispatch(contextNoise, frame);

        contextNoise.finish(mComputeQueue.incrementFence());

        graphics::CommandList batchedComputeCommandLists[1] = { contextNoise.commandList() };
        uint64_t computeFence = mComputeQueue.executeCommandLists(batchedComputeCommandLists);
        mComputeQueue.waitForFence(computeFence);
        mGraphicsQueue.stallForQueue(mComputeQueue);

        // WATER PASS
        graphics::GraphicContext& contextWater = aquireGraphicContext("Water_Graphic_Context");

        mWaterPass.getInputOutputs(contextWater, mResourceManager);
        mWaterPass.render(contextWater, frame);

        contextWater.finish(mGraphicsQueue.incrementFence());

        // PRESENT PASS
        graphics::GraphicContext& contextPresent = aquireGraphicContext("Present_Graphic_Context");

        contextPresent.transitionBarrier(mSwapchain.getBackBufferAt(frameI), graphics::GPUResourceStates::RenderTarget);

        mPresentPass.getInputOutputs(contextPresent, mResourceManager);
        mPresentPass.render(contextPresent, frame, mSwapchain.getBackBufferViewAt(frameI));

        contextPresent.transitionBarrier(mSwapchain.getBackBufferAt(frameI), graphics::GPUResourceStates::Present);
        contextPresent.finish(mGraphicsQueue.incrementFence());

        // EXECUTE COMMAND LISTS
        graphics::CommandList batchedGraphicsCommandLists[2] = { contextWater.commandList(), contextPresent.commandList() };
        uint64_t graphicsFence = mGraphicsQueue.executeCommandLists(batchedGraphicsCommandLists, 2);
        mGraphicsQueue.waitForFence(graphicsFence);

        // PRESENT
        mSwapchain.present(1);

        discardContext(contextWater);
        discardContext(contextPresent);
        discardContext(contextNoise);
    }

    Renderer::~Renderer()
    {
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        graphics::ShaderInput::freeAllRootSignatures();
    }

    void Renderer::setupDebugUI(const HWND windowHandle)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        style.FontScaleDpi = main_scale;
        io.ConfigDpiScaleFonts = true;
        io.ConfigDpiScaleViewports = true;

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(windowHandle);

        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = g_pd3dDevice;
        init_info.CommandQueue = g_pd3dCommandQueue;
        init_info.NumFramesInFlight = APP_NUM_FRAMES_IN_FLIGHT;
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
        // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
        // (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
        init_info.SrvDescriptorHeap = mResourceManager.getDescriptorHeap(graphics::DescriptorHeapWithAllocator::DescriptorsCategory::DebugGPU).get();
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return g_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle); };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) { return g_pd3dSrvDescHeapAlloc.Free(cpu_handle, gpu_handle); };
        ImGui_ImplDX12_Init(&init_info);
    }

    void Renderer::renderDebugUI(graphics::GraphicContext& context)
    {
        ImGui::Render();

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), context.commandList().get());
    }

    void Renderer::renderEndDebugUI()
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    void Renderer::cleanupDebugUI()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}
