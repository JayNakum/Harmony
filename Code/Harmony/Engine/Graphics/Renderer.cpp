#include "Renderer.h"

#include "ShaderInput.h"

namespace engine
{
    Renderer::Renderer(const HWND windowHandle)
        : mDevice(),
        mCopyQueue(mDevice, {"Renderer_Copy_Queue", graphics::CommandListType::Copy}),
        mComputeQueue(mDevice, {"Renderer_Compute_Queue", graphics::CommandListType::Compute}),
        mGraphicsQueue(mDevice, {"Renderer_Graphics_Queue", graphics::CommandListType::Direct}),
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
        mDebugUI(windowHandle, mDevice, mGraphicsQueue),
        mPresentPass(mDevice, mResourceManager),
        mWaterPass(mDevice, mResourceManager),
        mNoisePass(mDevice, mResourceManager)
    {
        graphics::CopyContext& copyContext = aquireCopyContext("Initial_Copy_Context");

        mNoisePass.copyDataToGPU(copyContext, mResourceManager);
        mWaterPass.copyDataToGPU(copyContext, mResourceManager);
        mPresentPass.copyDataToGPU(copyContext, mResourceManager);

        copyContext.finish(mCopyQueue.incrementFence());

        graphics::CommandList batchedComputeCommandLists[1] = { copyContext.commandList() };
        mCopyQueue.executeCommandLists(batchedComputeCommandLists);
        mCopyQueue.waitForIdle();

        discardContext(copyContext);

        mNoisePass.freeGPUUploadData(mResourceManager);
        mWaterPass.freeGPUUploadData(mResourceManager);
        mPresentPass.freeGPUUploadData(mResourceManager);

        mResourceManager.reset(graphics::HeapWithAllocator::ResourceCategory::UploadData);
    }

    void Renderer::resize(const unsigned int width, const unsigned int height)
    {
        mClientWidth = width;
        mClientHeight = height;

        mComputeQueue.waitForFence(mComputeQueue.incrementFence());
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        mNoisePass.resize(width, height, mDevice, mResourceManager);

        mWaterPass.resize(width, height, mDevice, mResourceManager);
        mPresentPass.resize(width, height, mDevice, mResourceManager, mSwapchain.isHDRSupported());

        mSwapchain.resize(
            mDevice, width, height,
            {
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU)
            }
        );
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

        const uint64_t noiseFence = mComputeQueue.incrementFence();
        contextNoise.finish(noiseFence);

        // WATER PASS
        mGraphicsQueue.stallForFence(mComputeQueue.getFence(), noiseFence);
        graphics::GraphicContext& contextWater = aquireGraphicContext("Water_Graphic_Context");

        mWaterPass.getInputOutputs(contextWater, mResourceManager);
        mWaterPass.render(contextWater, frame);

        const uint64_t waterFence = mGraphicsQueue.incrementFence();
        contextWater.finish(waterFence);

        // DEBUG UI
        graphics::GraphicContext& contextDebugUI = aquireGraphicContext("DebugUI_Graphic_Context");

        mDebugUI.render(contextDebugUI);

        const uint64_t debugUIFence = mGraphicsQueue.incrementFence();
        contextDebugUI.finish(debugUIFence);

        // PRESENT PASS
        graphics::GraphicContext& contextPresent = aquireGraphicContext("Present_Graphic_Context");

        contextPresent.transitionBarrier(mSwapchain.getBackBufferAt(frameI), graphics::GPUResourceStates::RenderTarget);

        mPresentPass.getInputOutputs(contextPresent, mResourceManager);
        mPresentPass.render(contextPresent, frame, mSwapchain.getBackBufferViewAt(frameI));

        contextPresent.transitionBarrier(mSwapchain.getBackBufferAt(frameI), graphics::GPUResourceStates::Present);

        const uint64_t presentFence = mGraphicsQueue.incrementFence();
        contextPresent.finish(presentFence);

        // EXECUTE COMMAND LISTS
        graphics::CommandList batchedComputeCommandLists[1] = { contextNoise.commandList() };
        const uint64_t computeFence = mComputeQueue.executeCommandLists(batchedComputeCommandLists);

        graphics::CommandList batchedGraphicsCommandLists[3] = { contextWater.commandList(), contextDebugUI.commandList(), contextPresent.commandList() };
        const uint64_t graphicsFence = mGraphicsQueue.executeCommandLists(batchedGraphicsCommandLists, 3);

        // PRESENT
        mSwapchain.present(1);
        mDevice.getDeviceRemovedReason();

        discardContext(contextWater);
        discardContext(contextPresent);
        discardContext(contextNoise);
        discardContext(contextDebugUI);
    }

    Renderer::~Renderer()
    {
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        graphics::ShaderInput::freeAllRootSignatures();
    }
}
