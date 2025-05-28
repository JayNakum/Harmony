#include "Renderer.h"

#include "ShaderInput.h"

namespace engine
{
    Renderer::Renderer(const HWND windowHandle)
        : mDevice(),
        mGraphicsQueue(mDevice, {"global graphics queue", graphics::CommandListType::Direct}),
        mCommandContextPool(),
        mSwapchain(
            mDevice, mGraphicsQueue,
            {
                "swapchain",
                windowHandle
            },
            {
                mCurrentRTVPage.allocate(1),
                mCurrentRTVPage.allocate(1),
                mCurrentRTVPage.allocate(1)
            }
        )
    {
        graphics::GraphicContext& initContext = aquireGraphicContext("init context");

        graphics::dx12::RenderTarget::Description description{};
        description.textureDescription.name = "water render target";
        description.textureDescription.clearColor[0] = 0.118f;
        description.textureDescription.clearColor[1] = 0.106f;
        description.textureDescription.clearColor[2] = 0.180f;
        description.textureDescription.clearColor[3] = 1.0f;
        waterRT = new graphics::dx12::RenderTarget(description);
        waterRT->getTexture().createCommitted(initContext.getDevice());
        waterSRV = mShaderViewsHeap.allocatePage().allocate(1);

        waterRTV = mCurrentRTVPage.allocate(1);
        waterRT->createRenderTargetView(initContext.getDevice(), waterRTV);

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

        if (!mCurrentRTVPage.canAllocate(3))
        {
            mCurrentRTVPage = mRTVDescriptorHeapCPU.allocatePage();
        }
        mSwapchain.resize(mDevice, width, height, { mCurrentRTVPage.allocate(1), mCurrentRTVPage.allocate(1), mCurrentRTVPage.allocate(1) });
    }

    void Renderer::render(const GraphicScene& scene)
    {
        // WATER PASS
        graphics::GraphicContext& contextWater = aquireGraphicContext("water");

        contextWater.transitionResource(waterRT->getTexture(), graphics::GPUResourceStates::RenderTarget, true);

        renderPass::Water::InputOutputs waterIO{
            waterRTV
        };
        mWaterPass->render(contextWater, waterIO);

        waterRT->getTexture().createShaderResourceView(mDevice, waterSRV,
            graphics::dx12::GPUTexture::ShaderResourceViewInfo::getSRVInfo(
                waterRT->getTexture().describeResource()
            ));

        contextWater.finish(mGraphicsQueue.incrementFence());

        // PRESENT PASS
        graphics::GraphicContext& contextPresent = aquireGraphicContext("present");

        contextPresent.transitionResource(waterRT->getTexture(), graphics::GPUResourceStates::PixelShaderResource, true);

        contextPresent.transitionResource(mSwapchain.getCurrentBackBuffer().getTexture(), graphics::GPUResourceStates::RenderTarget, true);

        renderPass::Present::InputOutputs presentIO{
        	mSwapchain.getCurrentBackBufferView(),
            waterSRV
        };
        mPresentPass->render(contextPresent, presentIO);

        contextPresent.transitionResource(mSwapchain.getCurrentBackBuffer().getTexture(), graphics::GPUResourceStates::Present, true);

        contextPresent.finish(mGraphicsQueue.incrementFence());

        graphics::dx12::CommandList batchedCommandLists[2] = { contextWater.commandList(), contextPresent.commandList() };
        uint64_t fence = mGraphicsQueue.executeCommandLists(batchedCommandLists, 2);

        mSwapchain.present(1);

        mGraphicsQueue.waitForFence(fence);

        discardGraphicContext(contextWater);
        discardGraphicContext(contextPresent);
    }

    Renderer::~Renderer()
    {
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        graphics::ShaderInput::msRootSignatures.clear();

        delete waterRT;
        delete mWaterPass;
        delete mPresentPass;
    }
}

namespace engine
{
    ResourceManager::ResourceManager(graphics::dx12::Device& device)
        :
        mRTVDescriptorHeapCPU(
            device,
            {
                "cpu visible render target view descriptor heap",
                graphics::DescriptorHeapType::RenderTarget,
                512,
                graphics::DescriptorHeapFlags::CPUVisible
            }
        ),
        mCurrentRTVPage(mRTVDescriptorHeapCPU.allocatePage()),
        mShaderViewsHeap(device),
        mCurrentShaderViewsPage(mShaderViewsHeap.allocatePage())
    {}

    const graphics::dx12::DescriptorHandle ResourceManager::create(graphics::CommandContext& context, const CreateInfo& createInfo)
    {
        graphics::dx12::Device& device = context.getDevice();
        createInfo.resourceDescription.initialState = graphics::GPUResourceStates::CopyDestination;
        mDescriptors.emplace(createInfo.id, mCurrentShaderViewsPage.allocate());
        graphics::dx12::GPUTexture& resource = mTextures.at(createInfo.id);
        resource.createCommitted(device);

        mTextures.emplace(createInfo.id, createInfo.resourceDescription);
        graphics::dx12::DescriptorHandle& handle = mDescriptors.at(createInfo.id);
        resource.createShaderResourceView(device, handle, graphics::dx12::GPUTexture::ShaderResourceViewInfo::getSRVInfo(resource.describeResource()));


        return handle;
    }

    const graphics::dx12::DescriptorHandle ResourceManager::read(const std::string& id, graphics::CommandContext& context, const graphics::dx12::GPUTexture::ShaderResourceViewInfo& view)
    {
        return graphics::dx12::DescriptorHandle();
    }

    const graphics::dx12::DescriptorHandle ResourceManager::write(const std::string& id, graphics::CommandContext& context, const graphics::dx12::GPUTexture::ShaderResourceViewInfo& view)
    {
        return graphics::dx12::DescriptorHandle();
    }

    const graphics::dx12::DescriptorHandle ResourceManager::createRT(const std::string& id, graphics::CommandContext& context, const graphics::dx12::RenderTarget::Description createDescription)
    {
        graphics::dx12::DescriptorHandle handle = createRTVs(1);



        return handle;
    }

    const graphics::GPUResourceStates ResourceManager::getResourceStates(const graphics::ShaderVisibility visibility)
    {

    }
}
