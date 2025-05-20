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

    struct Vertex
    {
        float position[4];
        float color[4];
    };

    void Renderer::render()
    {
        // WATER PASS
        graphics::GraphicContext& contextWater = aquireGraphicContext("water");

        Vertex triangleVertices[] =
        {
            { { 0.0f, 0.25f, 0.0f, 1.0f }, { 0.835f, 0.518f, 0.988f, 1.0f } },
            { { 0.25f, -0.25f, 0.0f, 1.0f }, { 0.694f, 0.392f, 0.804f, 1.0f } },
            { { -0.25f, -0.25f, 0.0f, 1.0f }, { 0.796f, 0.643f, 0.914f, 1.0f } }
        };

        size_t numBytes = sizeof(triangleVertices);

        graphics::dx12::UploadBuffer intermediateBuffer(contextWater.getDevice(), { "intermediate buffer", numBytes });
        {
            void* intermediateDst = intermediateBuffer.map();
            memcpy(intermediateDst, triangleVertices, numBytes);
            intermediateBuffer.unmap();
        }
        graphics::dx12::GPUResource::Description createDesc;
        createDesc.name = "vertex buffer";
        createDesc.width = numBytes;
        createDesc.dimension = graphics::GPUResourceDimension::Buffer;
        graphics::dx12::ByteAddressBuffer vertexBuffer(createDesc);

        vertexBuffer.createCommitted(contextWater.getDevice());

        //contextWater.transitionResource(vertexBuffer, graphics::GPUResourceStates::CopyDestination, true);

        contextWater.transitionResource(intermediateBuffer, graphics::GPUResourceStates::CopySource, true);
        contextWater.copyBufferRegion(vertexBuffer, 0, intermediateBuffer, 0, numBytes);
        contextWater.transitionResource(vertexBuffer, graphics::GPUResourceStates::VertexAndConstantBuffer);

        graphics::dx12::BufferView vertexBufferView = vertexBuffer.vertexBufferView(sizeof(Vertex));

        graphics::dx12::RenderTarget::Description description = mRenderTargetDescription;
        description.textureDescription.name = "water render target";
        graphics::dx12::RenderTarget waterRT(description);
        waterRT.getTexture().createCommitted(contextWater.getDevice());
        graphics::dx12::DescriptorHandle waterRTV = mRTVPage.allocate(1);
        waterRT.createRenderTargetView(contextWater.getDevice(), waterRTV);
        contextWater.transitionResource(waterRT.getTexture(), graphics::GPUResourceStates::RenderTarget);

        renderPass::Water::InputOutputs waterIO{
            vertexBufferView,
            waterRTV
        };
        waterPass.render(contextWater, waterIO);

        // PRESENT PASS
        graphics::GraphicContext& contextPresent = aquireGraphicContext("present");

        contextPresent.transitionResource(waterRT.getTexture(), graphics::GPUResourceStates::PixelShaderResource);

        contextPresent.transitionResource(mSwapchain.getCurrentBackBuffer().getTexture(), graphics::GPUResourceStates::RenderTarget);

        renderPass::Present::InputOutputs presentIO{
            mClientWidth, mClientHeight,
            waterRT.getTexture(),
            mSwapchain.getCurrentBackBufferView()
        };
        presentPass.render(contextPresent, presentIO);

        contextPresent.transitionResource(mSwapchain.getCurrentBackBuffer().getTexture(), graphics::GPUResourceStates::Present);

        mGraphicsQueue.executeCommandList(contextWater.commandList());
        mGraphicsQueue.executeCommandList(contextPresent.commandList());

        mSwapchain.present();

        mGraphicsQueue.waitForIdle();
    }

    Renderer::~Renderer()
    {
    }
}

