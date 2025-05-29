#include "Renderer.h"

#include "ShaderInput.h"

namespace engine
{
    Renderer::Renderer(const HWND windowHandle)
        : mDevice(),
        mGraphicsQueue(mDevice, {"global graphics queue", graphics::CommandListType::Direct}),
        mCommandContextPool(),
        mShaderViewsHeap(mDevice),
        mRTVDescriptorHeapCPU(
            mDevice,
            {
                "cpu visible render target view descriptor heap",
                graphics::DescriptorHeapType::RenderTarget,
                1024,
                graphics::DescriptorHeapFlags::CPUVisible
            }
        ),
        mSwapchainRTVPage(mRTVDescriptorHeapCPU.allocatePage()),
        mSwapchain(
            mDevice, mGraphicsQueue,
            {
                "swapchain",
                windowHandle
            },
            {
                mSwapchainRTVPage.allocate(1),
                mSwapchainRTVPage.allocate(1),
                mSwapchainRTVPage.allocate(1)
            }
        )
    {
        graphics::GraphicContext& initContext = aquireGraphicContext("init context");

        mWaterPass = new renderPass::Water(initContext);

        mPresentPass = new renderPass::Present(initContext);

        initContext.finish(mGraphicsQueue.incrementFence());

        graphics::dx12::CommandList batchedCommandLists[1] = { initContext.commandList() };
        uint64_t fence = mGraphicsQueue.executeCommandLists(batchedCommandLists);
        mGraphicsQueue.waitForFence(fence);

        discardGraphicContext(initContext);
    }

    void Renderer::resize(const unsigned int width, const unsigned int height)
    {
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        mSwapchain.resize(mDevice, width, height, { mSwapchainRTVPage.allocate(1), mSwapchainRTVPage.allocate(1), mSwapchainRTVPage.allocate(1) });
    }

    void Renderer::render(const FrameConstants frame, const PassConstants pass)
    {
        // WATER PASS
        graphics::GraphicContext& contextWater = aquireGraphicContext("water");

        renderPass::Water::Inputs waterInputs{
            frame,
            pass
        };
        renderPass::Water::Outputs waterOutput = mWaterPass->render(contextWater, waterInputs);

        contextWater.finish(mGraphicsQueue.incrementFence());

        // PRESENT PASS
        graphics::GraphicContext& contextPresent = aquireGraphicContext("present");

        renderPass::Present::Inputs presentInputs{
            frame,
            pass,
            waterOutput.outputRender,
            mSwapchain.getCurrentBackBuffer().getTexture(),
            mSwapchain.getCurrentBackBufferView()
        };
        mPresentPass->render(contextPresent, presentInputs);

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

        graphics::ShaderInput::msRootSignatures.clear();

        delete mWaterPass;
        delete mPresentPass;
    }
}
