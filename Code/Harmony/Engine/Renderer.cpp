#include "Renderer.h"

#include "ShaderInput.h"

struct Vertex
{
    float position[4];
    float color[4];
};

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
                graphics::DescriptorHeapType::RenderTarget,
                512,
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
            {
                "default render target",
                clientWidth,
                clientHeight,
                1, 1,
                graphics::GPUResourceDimension::Texture2D,
                graphics::DefaultFormats::R8G8B8A8_UNORM,
                graphics::GPUResourceFlags::AllowRenderTarget,
            }
        }
    {
        graphics::GraphicContext& initContext = aquireGraphicContext("init context");

        Vertex triangleVertices[] =
        {
            { { 0.0f, 0.25f, 0.0f, 1.0f }, { 0.835f, 0.518f, 0.988f, 1.0f } },
            { { 0.25f, -0.25f, 0.0f, 1.0f }, { 0.694f, 0.392f, 0.804f, 1.0f } },
            { { -0.25f, -0.25f, 0.0f, 1.0f }, { 0.796f, 0.643f, 0.914f, 1.0f } }
        };

        size_t numBytes = sizeof(triangleVertices);

        graphics::dx12::UploadBuffer intermediateBuffer(initContext.getDevice(), { "intermediate buffer", numBytes });
        {
            void* intermediateDst = intermediateBuffer.map();
            memcpy(intermediateDst, triangleVertices, numBytes);
            intermediateBuffer.unmap();
        }
        graphics::dx12::GPUResource::Description createDesc;
        createDesc.name = "vertex buffer";
        createDesc.width = numBytes;
        createDesc.dimension = graphics::GPUResourceDimension::Buffer;
        vertexBuffer = new graphics::dx12::ByteAddressBuffer(createDesc);

        vertexBuffer->createCommitted(initContext.getDevice());


        //initContext.transitionResource(vertexBuffer, graphics::GPUResourceStates::CopyDestination, true);

        initContext.transitionResource(intermediateBuffer, graphics::GPUResourceStates::CopySource, true);
        initContext.copyBufferRegion(*vertexBuffer, 0, intermediateBuffer, 0, numBytes);
        initContext.transitionResource(*vertexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer, true);


        graphics::dx12::RenderTarget::Description description = mRenderTargetDescription;
        description.textureDescription.name = "water render target";
        description.textureDescription.clearColor[0] = 0.118f;
        description.textureDescription.clearColor[1] = 0.106f;
        description.textureDescription.clearColor[2] = 0.180f;
        description.textureDescription.clearColor[3] = 1.0f;
        waterRT = new graphics::dx12::RenderTarget(description);
        waterRT->getTexture().createCommitted(initContext.getDevice());
        waterSRV = mShaderViewsHeap.allocatePage().allocate(1);

        waterRTV = mRTVPage.allocate(1);
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

        graphics::dx12::BufferView vertexBufferView = vertexBuffer->vertexBufferView(sizeof(Vertex));

        contextWater.transitionResource(waterRT->getTexture(), graphics::GPUResourceStates::RenderTarget, true);

        renderPass::Water::InputOutputs waterIO{
            mClientWidth, mClientHeight,
            vertexBufferView,
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
        	mClientWidth, mClientHeight,
            waterSRV,
        	mSwapchain.getCurrentBackBufferView()
        };
        mPresentPass->render(contextPresent, presentIO);

        contextPresent.transitionResource(mSwapchain.getCurrentBackBuffer().getTexture(), graphics::GPUResourceStates::Present, true);

        contextPresent.finish(mGraphicsQueue.incrementFence());

        graphics::dx12::CommandList batchedCommandLists[2] = { contextWater.commandList(), contextPresent.commandList() };
        uint64_t fence = mGraphicsQueue.executeCommandLists(batchedCommandLists, 2);

        mSwapchain.present();

        mGraphicsQueue.waitForFence(fence);

        discardGraphicContext(contextWater);
        discardGraphicContext(contextPresent);
    }

    Renderer::~Renderer()
    {
        mGraphicsQueue.waitForFence(mGraphicsQueue.incrementFence());

        graphics::ShaderInput::msRootSignatures.clear();

        delete vertexBuffer;
        delete waterRT;
        delete mWaterPass;
        delete mPresentPass;
    }
}
