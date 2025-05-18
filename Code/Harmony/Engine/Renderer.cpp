#include "Renderer.h"

namespace engine
{
    Renderer::Renderer(const HWND handle, unsigned int clientWidth, unsigned int clientHeight)
        : mDevice(),
        mGraphicsQueue(mDevice, {"global graphics queue", graphics::CommandListType::Direct}),
        mCommandContextPool(),
        mShaderViewsHeap(mDevice),
        mRTVDescriptorHeapCPU(
            mDevice,
            {
                "cpu visible render target view descriptor heap",
                graphics::DescriptorHeapType::ShaderViews,
                6,
                graphics::DescriptorHeapFlags::CPUVisible
            }
        ),
        mRTVPage(mRTVDescriptorHeapCPU.allocatePage()),
        mSwapchain(
            mDevice, mGraphicsQueue,
            {
                "swapchain",
                handle
            },
            {
                mRTVPage.allocate(1),
                mRTVPage.allocate(1),
                mRTVPage.allocate(1)
            }
        ),
        mClientWidth(clientWidth), mClientHeight(clientHeight),
        mRenderTargetDescription{
            graphics::dx12::RenderTarget::Type::Color,
            "default render target",
            clientWidth,
            clientHeight,
            1, 1,
            graphics::GPUResourceDimension::Texture2D,
            graphics::DefaultFormats::R8G8B8A8_UNORM,
            graphics::GPUResourceFlags::AllowRenderTarget,
        }
    {}

    void Renderer::resize(const unsigned int width, const unsigned int height)
    {
        mRenderTargetDescription.textureDescription.width = width;
        mRenderTargetDescription.textureDescription.height = height;

        if (!mRTVPage.canAllocate(3))
        {
            mRTVPage = mRTVDescriptorHeapCPU.allocatePage();
        }
        mSwapchain.resize(mDevice, width, height, { mRTVPage.allocate(1), mRTVPage.allocate(1), mRTVPage.allocate(1) });
    }

    void Renderer::render()
    {
        // WATER PASS
        graphics::GraphicContext& contextWater = aquireGraphicContext("water");

        graphics::dx12::RenderTarget::Description description = mRenderTargetDescription;
        description.textureDescription.name = "water render target";
        graphics::dx12::RenderTarget waterRT(description);
        graphics::dx12::DescriptorHandle waterRTV = mRTVPage.allocate(1);

        waterRT.createRenderTargetView(
            contextWater.getDevice(),
            waterRTV
        );
        contextWater.transitionResource(waterRT.getTexture(), graphics::GPUResourceStates::RenderTarget);
        contextWater.setRenderTargets(waterRTV);

        renderPass::Water::InputOutputs waterIO{};
        waterPass.render(contextWater, waterIO);

        // PRESENT PASS
        graphics::GraphicContext& contextPresent = aquireGraphicContext("present");

        contextPresent.transitionResource(waterRT.getTexture(), graphics::GPUResourceStates::PixelShaderResource);

        contextPresent.transitionResource(mSwapchain.getCurrentBackBuffer().getTexture(), graphics::GPUResourceStates::RenderTarget);
        contextPresent.setRenderTargets(mSwapchain.getCurrentBackBufferView());

        renderPass::Present::InputOutputs presentIO{
            mClientWidth, mClientHeight,
            waterRT.getTexture()
        };
        presentPass.render(contextPresent, presentIO);

        contextPresent.transitionResource(mSwapchain.getCurrentBackBuffer().getTexture(), graphics::GPUResourceStates::Present);
    }

    Renderer::~Renderer()
    {
    }
}

