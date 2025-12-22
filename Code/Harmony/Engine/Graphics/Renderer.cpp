#include "Renderer.h"

#include "ShaderInput.h"

namespace engine
{
    Renderer::Renderer(const HWND windowHandle)
        : mDevice(),
        mCopyQueue(mDevice, { "Renderer_Copy_Queue", graphics::CommandListType::Copy }),
        mComputeQueue(mDevice, { "Renderer_Compute_Queue", graphics::CommandListType::Compute }),
        mGraphicsQueue(mDevice, { "Renderer_Graphics_Queue", graphics::CommandListType::Direct }),
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
        mRenderParticlesPass(mDevice, mResourceManager),
        // mWaterPass(mDevice, mResourceManager),
        // mNoisePass(mDevice, mResourceManager),
        mSimulateParticlesPass(mDevice, mResourceManager)
    {
        graphics::CopyContext& contextCopy = aquireCopyContext("Initial_Copy_Context");

        // mNoisePass.copyDataToGPU(contextCopy, mResourceManager);
        // mWaterPass.copyDataToGPU(contextCopy, mResourceManager);
        mSimulateParticlesPass.copyDataToGPU(contextCopy, mResourceManager);
        mRenderParticlesPass.copyDataToGPU(contextCopy, mResourceManager);
        mPresentPass.copyDataToGPU(contextCopy, mResourceManager);

        contextCopy.finish(mCopyQueue.incrementFence());

        graphics::CommandList* batchedCopyCommandLists[1] = { &contextCopy.commandList() };
        mCopyQueue.executeCommandLists(batchedCopyCommandLists);
        mCopyQueue.waitForIdle();

        discardContext(contextCopy);

#if !defined(HDEBUG)
        // mNoisePass.freeGPUUploadData(mResourceManager);
        // mWaterPass.freeGPUUploadData(mResourceManager);
        mSimulateParticlesPass.freeGPUUploadData(mResourceManager);
        mRenderParticlesPass.freeGPUUploadData(mResourceManager);
        mPresentPass.freeGPUUploadData(mResourceManager);
#endif

        mResourceManager.reset(graphics::HeapWithAllocator::ResourceCategory::UploadData);
    }

    void Renderer::resize(const unsigned int width, const unsigned int height)
    {
        mClientWidth = width;
        mClientHeight = height;

        mComputeQueue.waitForFence(mComputeQueue.incrementFence());
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        // mNoisePass.resize(width, height, mDevice, mResourceManager);

        // mWaterPass.resize(width, height, mDevice, mResourceManager);
        mSimulateParticlesPass.resize(width, height, mDevice, mResourceManager);
        mRenderParticlesPass.resize(width, height, mDevice, mResourceManager);
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
        // mWaterPass.update(timeStep);
        mRenderParticlesPass.update(timeStep);
        mPresentPass.update(timeStep);

        // mNoisePass.update(timeStep);
        mSimulateParticlesPass.update(timeStep);
    }

    void Renderer::render(const FrameConstants frame)
    {
        const unsigned int frameI = mSwapchain.getCurrentBackBufferIndex();

        // graphics::CopyContext& contextCopy = aquireCopyContext("Default_Copy_Context");

        // NOISE PASS
        // graphics::ComputeContext& contextNoise = aquireComputeContext("Noise_Compute_Context");
        //
        // mNoisePass.getInputOutputs(contextNoise, mResourceManager);
        // mNoisePass.dispatch(contextNoise, frame);
        //
        // const uint64_t noiseFence = mComputeQueue.incrementFence();
        // contextNoise.finish(noiseFence);

        // WATER PASS
        // mGraphicsQueue.stallForFence(mComputeQueue.getFence(), noiseFence);
        // graphics::GraphicContext& contextWater = aquireGraphicContext("Water_Graphic_Context");
        //
        // if (mWaterPass.needsCopyDataToGPU())
        // {
        //     mWaterPass.copyDataToGPU(contextCopy, mResourceManager);
        //
        //     const uint64_t copyFence = mCopyQueue.incrementFence();
        //     mGraphicsQueue.stallForFence(mCopyQueue.getFence(), copyFence);
        // }
        //
        // mWaterPass.getInputOutputs(contextWater, mResourceManager);
        // mWaterPass.render(contextWater, frame);
        //
        // const uint64_t waterFence = mGraphicsQueue.incrementFence();
        // contextWater.finish(waterFence);

        // SIMULATE PARTICLES PASS
        graphics::ComputeContext& contextSimulateParticles = aquireComputeContext("SimulateParticles_Compute_Context");

        mSimulateParticlesPass.getInputOutputs(contextSimulateParticles, mResourceManager);
        mSimulateParticlesPass.dispatch(contextSimulateParticles, frame);

        const uint64_t simulateParticlesFence = mComputeQueue.incrementFence();
        contextSimulateParticles.finish(simulateParticlesFence);

        // RENDER PARTICLES PASS
        mGraphicsQueue.stallForFence(mComputeQueue.getFence(), simulateParticlesFence);
        graphics::GraphicContext& contextParticles = aquireGraphicContext("Particles_Graphic_Context");

        mRenderParticlesPass.getInputOutputs(contextParticles, mResourceManager);
        mRenderParticlesPass.render(contextParticles, frame);

        const uint64_t waterFence = mGraphicsQueue.incrementFence();
        contextParticles.finish(waterFence);

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
        graphics::CommandList* batchedComputeCommandLists0[1] = { &contextSimulateParticles.commandList() };
        mComputeQueue.executeCommandLists(batchedComputeCommandLists0);

        graphics::CommandList* batchedGraphicsCommandLists1[3] = { &contextParticles.commandList(), &contextDebugUI.commandList(), &contextPresent.commandList() };
        mGraphicsQueue.executeCommandLists(batchedGraphicsCommandLists1, 3);

        // PRESENT
        mSwapchain.present(1);
        mDevice.getDeviceRemovedReason();

        mGraphicsQueue.waitForIdle();

        // discardContext(contextNoise);
        // discardContext(contextWater);
        discardContext(contextSimulateParticles);
        discardContext(contextParticles);
        discardContext(contextPresent);
        discardContext(contextDebugUI);
    }

    Renderer::~Renderer()
    {
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

#if defined(HDEBUG)
        // mNoisePass.freeGPUUploadData(mResourceManager);
        // mWaterPass.freeGPUUploadData(mResourceManager);
        mSimulateParticlesPass.freeGPUUploadData(mResourceManager);
        mRenderParticlesPass.freeGPUUploadData(mResourceManager);
        mPresentPass.freeGPUUploadData(mResourceManager);
#endif

        graphics::ShaderInput::freeAllRootSignatures();
    }
}
