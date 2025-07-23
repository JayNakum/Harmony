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
        mPresentPass(mDevice, mResourceManager)
    {
        graphics::GraphicContext& initContext = aquireGraphicContext("init context");

        mWaterPass.copyDataToGPU(initContext, mResourceManager);
        mPresentPass.copyDataToGPU(initContext, mResourceManager);

        initContext.finish(mGraphicsQueue.incrementFence());

        graphics::dx12::CommandList batchedCommandLists[1] = { initContext.commandList() };
        uint64_t fence = mGraphicsQueue.executeCommandLists(batchedCommandLists);
        mGraphicsQueue.waitForFence(fence);

        discardGraphicContext(initContext);
        mWaterPass.freeGPUUploadData(mResourceManager);
        mPresentPass.freeGPUUploadData(mResourceManager);
        mResourceManager.reset(graphics::HeapWithAllocator::ResourceCategory::Upload);
    }

    void Renderer::resize(const unsigned int width, const unsigned int height)
    {
        mClientWidth = width;
        mClientHeight = height;

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
    }

    void Renderer::update(const double timeStep)
    {
        mWaterPass.update(timeStep);
        mPresentPass.update(timeStep);
    }

    void Renderer::render(const FrameConstants frame)
    {
        const unsigned int frameI = mSwapchain.getCurrentBackBufferIndex();

        // WATER PASS
        graphics::GraphicContext& contextWater = aquireGraphicContext("water");

        mWaterPass.getInputOutputs(contextWater, mResourceManager);
        mWaterPass.render(contextWater, frame);

        contextWater.finish(mGraphicsQueue.incrementFence());

        // PRESENT PASS
        graphics::GraphicContext& contextPresent = aquireGraphicContext("present");

        contextPresent.transitionBarrier(mSwapchain.getBackBufferAt(frameI), graphics::GPUResourceStates::RenderTarget);

        mPresentPass.getInputOutputs(contextPresent, mResourceManager);
        mPresentPass.render(contextPresent, frame, mSwapchain.getBackBufferViewAt(frameI));

        contextPresent.transitionBarrier(mSwapchain.getBackBufferAt(frameI), graphics::GPUResourceStates::Present);
        contextPresent.finish(mGraphicsQueue.incrementFence());

        // EXECUTE COMMAND LISTS
        graphics::dx12::CommandList batchedCommandLists[2] = { contextWater.commandList(), contextPresent.commandList() };
        uint64_t fence = mGraphicsQueue.executeCommandLists(batchedCommandLists, 2);

        // PRESENT
        mSwapchain.present(1);
        mGraphicsQueue.waitForFence(fence);

        discardGraphicContext(contextWater);
        discardGraphicContext(contextPresent);
    }

    Renderer::~Renderer()
    {
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        graphics::ShaderInput::freeAllRootSignatures();
    }
}
