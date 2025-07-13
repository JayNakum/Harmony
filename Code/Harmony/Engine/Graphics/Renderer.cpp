#include "Renderer.h"

#include "ShaderInput.h"

namespace engine
{
    Renderer::Renderer(const HWND windowHandle)
        : mDevice(),
        mGraphicsQueue(mDevice, {"global graphics queue", graphics::CommandListType::Direct}),
        mResourceManager(mDevice),
        mCommandContextPool(),
        mSwapchain(
            mDevice, mGraphicsQueue,
            {
                "swapchain",
                windowHandle
            },
            {
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::Category::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::Category::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::Category::RenderTargetCPU)
            }
        ),
        mClientWidth(0), mClientHeight(0)
    {
        graphics::GraphicContext& initContext = aquireGraphicContext("init context");

        mWaterPass = new renderPass::Water(initContext);

        mPresentPass = new renderPass::Present(initContext);

        initContext.finish(mGraphicsQueue.incrementFence());

        graphics::dx12::CommandList batchedCommandLists[1] = { initContext.commandList() };
        uint64_t fence = mGraphicsQueue.executeCommandLists(batchedCommandLists);
        mGraphicsQueue.waitForFence(fence);

        discardGraphicContext(initContext);

        mWaterPass->freeUploadResources();
    }

    void Renderer::resize(const unsigned int width, const unsigned int height)
    {
        mClientWidth = width;
        mClientHeight = height;

        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        mSwapchain.resize(
            mDevice, width, height,
            {
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::Category::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::Category::RenderTargetCPU),
                mResourceManager.allocateDescriptors(1, graphics::DescriptorHeapWithAllocator::Category::RenderTargetCPU)
            }
        );
    }

    void Renderer::render(const FrameConstants frame)
    {
        // WATER PASS
        graphics::GraphicContext& contextWater = aquireGraphicContext("water");

        renderPass::Water::Inputs waterInputs{
            frame,
            mClientWidth, mClientHeight
        };
        renderPass::Water::Outputs waterOutput = mWaterPass->render(contextWater, waterInputs);

        contextWater.finish(mGraphicsQueue.incrementFence());

        // PRESENT PASS
        graphics::GraphicContext& contextPresent = aquireGraphicContext("present");

        renderPass::Present::Inputs presentInputs{
            frame,
            mClientWidth, mClientHeight,
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

        graphics::ShaderInput::freeAllRootSignatures();

        delete mWaterPass;
        delete mPresentPass;
    }
}
