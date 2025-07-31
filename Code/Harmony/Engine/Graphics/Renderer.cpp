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

            graphics::dx12::CommandList batchedGraphicsCommandLists[1] = { initGraphicsContext.commandList() };
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

            graphics::dx12::CommandList batchedComputeCommandLists[1] = { initComputeContext.commandList() };
            uint64_t computeFence = mComputeQueue.executeCommandLists(batchedComputeCommandLists);
            mComputeQueue.waitForFence(computeFence);

            discardContext(initComputeContext);
            mNoisePass.freeGPUUploadData(mResourceManager);
        }

        mResourceManager.reset(graphics::HeapWithAllocator::ResourceCategory::Upload);
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

        graphics::dx12::CommandList batchedComputeCommandLists[1] = { contextNoise.commandList() };
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
        graphics::dx12::CommandList batchedGraphicsCommandLists[2] = { contextWater.commandList(), contextPresent.commandList() };
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
}
