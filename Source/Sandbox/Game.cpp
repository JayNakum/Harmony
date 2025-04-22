#include "common.h"

#include "Entrypoint.h"
#include "dx12.h"
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include "CommandListManager.h"
#include "DescriptorHeapWithAllocator.h"

namespace sandbox
{
    using namespace graphics;
    using namespace dx12;

    class Application : public harmony::Application
    {
        static constexpr unsigned int CLIENT_WIDTH = 1080;
        static constexpr unsigned int CLIENT_HEIGHT = 720;
    public:
        Application()
            : mWindow({L"Sandbox", CLIENT_WIDTH, CLIENT_HEIGHT}),
            mInputs(mWindow.getHandle()),
            mDevice(),
            mViewport(0.0f, 0.0f, static_cast<float>(CLIENT_WIDTH), static_cast<float>(CLIENT_HEIGHT)),
            mScissorRect(0, 0, static_cast<LONG>(CLIENT_WIDTH), static_cast<LONG>(CLIENT_HEIGHT)),
            mRtvHeap(mDevice, {"rtv descriptor heap", DescriptorHeapType::RenderTarget, 3, DescriptorHeapFlags::CPUVisible }),
            mDescriptorSize(mRtvHeap.getDescriptorSize()),
            mCommandListManager(mDevice)
        {
            RenderTarget::Description renderTargetDescription;
            {
                renderTargetDescription.type = RenderTarget::Type::Color;
                renderTargetDescription.textureDescription.width = CLIENT_WIDTH;
                renderTargetDescription.textureDescription.height = CLIENT_HEIGHT;
                renderTargetDescription.textureDescription.format = DefaultFormats::R8G8B8A8_UNORM;
                renderTargetDescription.textureDescription.name = "swapchain back buffer";
                renderTargetDescription.textureDescription.resourceFlags = GPUResourceFlags::AllowRenderTarget;
                renderTargetDescription.textureDescription.dimension = GPUResourceDimension::Texture2D;
            }
            {// Swapchain
                Swapchain::Description createDesc;
                {
                    createDesc.name = "swapchain";
                    createDesc.windowHandle = mWindow.getHandle();
                    createDesc.renderTargetDescription = renderTargetDescription;
                }

                DescriptorHandle rtvHandles = mRtvHeap.allocate(3);
                DescriptorHandle rtv0 = mRtvHeap[0];
                DescriptorHandle rtv1 = mRtvHeap[1];
                DescriptorHandle rtv2 = mRtvHeap[2];
                mSwapchain = new Swapchain(mDevice, mCommandListManager.getGraphicsQueue(), createDesc, {rtv0, rtv1, rtv2});
            }
        }

        void initialize() override
        {
            mCommandListManager.createNewCommandList(CommandListType::Direct, mCommandList, mCurrentCommandAllocator);

            {// Root Signature
                RootSignature::Description createDesc;
                createDesc.name = "root signature";
                createDesc.flags = RootSignatureFlags::AllowInputAssemblerInputLayout;
                mRootSignature = new RootSignature(mDevice, createDesc);
            }

            {// Pipeline State
                GraphicsPipelineState::Description createDesc;
                createDesc.name = "graphics pipeline state";
                createDesc.inputLayout = {
                    {"POSITION", DefaultFormats::R32G32B32A32_FLOAT, 0, InputClassification::PerVertex},
                    {"COLOR", DefaultFormats::R32G32B32A32_FLOAT, 16, InputClassification::PerVertex},
                };
                createDesc.rootSignature = mRootSignature;

                UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
                Blob vertexShader;
                Blob pixelShader;
                Blob errorBlob;
                {
                    HRESULT hResult = S_OK;

                    hResult = D3DCompileFromFile(L"D:\\Harmony\\Source\\Sandbox\\Shaders\\shader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
                    HASSERT(SUCCEEDED(hResult), "Failed to compile vertex shader.\n%s", errorBlob->GetBufferPointer());

                    hResult = D3DCompileFromFile(L"D:\\Harmony\\Source\\Sandbox\\Shaders\\shader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);
                    HASSERT(SUCCEEDED(hResult), "Failed to compile pixel shader.\n%s", errorBlob->GetBufferPointer());
                }
                createDesc.vertexShader = vertexShader;
                createDesc.pixelShader = pixelShader;

                createDesc.numRenderTargets = 1;
                createDesc.rtvFormats[0] = DefaultFormats::R8G8B8A8_UNORM;

                mPipelineState = new GraphicsPipelineState(mDevice, createDesc);
            }

            {// Vertex buffer
                Vertex triangleVertices[] =
                {
                    { { 0.0f, 0.25f, 0.0f, 1.0f }, { 0.835f, 0.518f, 0.988f, 1.0f } },
                    { { 0.25f, -0.25f, 0.0f, 1.0f }, { 0.694f, 0.392f, 0.804f, 1.0f } },
                    { { -0.25f, -0.25f, 0.0f, 1.0f }, { 0.796f, 0.643f, 0.914f, 1.0f } }
                };

                size_t numBytes = sizeof(triangleVertices);

                dx12::UploadBuffer intermediateBuffer(mDevice, {"intermediate buffer", numBytes });
                {
                    void* intermediateDst = intermediateBuffer.map();
                    memcpy(intermediateDst, triangleVertices, numBytes);
                    intermediateBuffer.unmap();
                }
                GPUResource::Description createDesc;
                createDesc.name = "vertex buffer";
                createDesc.width = numBytes;
                createDesc.dimension = GPUResourceDimension::Buffer;
                mVertexBuffer = new ByteAddressBuffer(createDesc);

                mVertexBuffer->createCommitted(mDevice);

                commandList().transitionResource(*mVertexBuffer, GPUResourceStates::CopyDestination, true);
                commandList()->CopyBufferRegion(mVertexBuffer->get(), 0, (ID3D12Resource*)intermediateBuffer.get(), 0, numBytes);
                commandList().transitionResource(*mVertexBuffer, GPUResourceStates::VertexAndConstantBuffer, true);
                mCommandListManager.getGraphicsQueue().executeCommandList(commandList());
            }

            mCommandListManager.idleGPU();
        }

        void run() override
        {
            HRESULT hResult = S_OK;
            while (!(mInputs.isPressed(core::Inputs::Digital::Key_escape)) &&
                   mWindow.pollEvents())
            {
                mInputs.update(0);

                hResult = commandAllocator()->Reset();
                HASSERT(SUCCEEDED(hResult), "Failed to reset command allocator");

                hResult = commandList()->Reset(commandAllocator().get(), mPipelineState->get());
                HASSERT(SUCCEEDED(hResult), "Failed to reset command list");

                commandList()->SetGraphicsRootSignature(mRootSignature->get());
                commandList()->RSSetViewports(1, &mViewport);
                commandList()->RSSetScissorRects(1, &mScissorRect);

                commandList().transitionResource(mSwapchain->getCurrentBackBuffer().getTexture(), GPUResourceStates::RenderTarget, true);
                {
                    DescriptorHandle rtvHandle = mRtvHeap[mSwapchain->getCurrentBackBufferIndex()];
                    commandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

                    const float clearColor[] = { 0.118f, 0.106f, 0.180f, 1.0f };
                    commandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
                    commandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    D3D12_VERTEX_BUFFER_VIEW vertexBufferView = BufferView::toD3DVertexBufferView(mVertexBuffer->vertexBufferView(sizeof(Vertex)));
                    commandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
                    commandList()->DrawInstanced(3, 1, 0, 0);
                }
                commandList().transitionResource(mSwapchain->getCurrentBackBuffer().getTexture(), GPUResourceStates::Present, true);

                mCommandListManager.getGraphicsQueue().executeCommandList(commandList());

                mSwapchain->present();

                mCommandListManager.idleGPU();
            }
        }

        void deinitialize() override
        {
            mCommandListManager.idleGPU();
        }

        ~Application()
        {
            delete mVertexBuffer;
            delete mPipelineState;
            delete mRootSignature;

            delete mSwapchain;
        }

    private:
        windows::Window mWindow;
        Device mDevice;
        windows::Inputs mInputs;

        CD3DX12_VIEWPORT mViewport;
        CD3DX12_RECT mScissorRect;

        DescriptorHeapWithAllocator mRtvHeap;
        size_t mDescriptorSize = 0;
        Swapchain* mSwapchain = nullptr;

        CommandListManager mCommandListManager;
        inline CommandList& commandList() { return *mCommandList; }
        dx12::CommandList* mCommandList = nullptr;
        inline CommandAllocator& commandAllocator() { return *mCurrentCommandAllocator; }
        dx12::CommandAllocator* mCurrentCommandAllocator = nullptr;

        RootSignature* mRootSignature = nullptr;
        GraphicsPipelineState* mPipelineState = nullptr;

        Blob mVertexShader;
        Blob mPixelShader;

        struct Vertex
        {
            DirectX::XMFLOAT4 position;
            DirectX::XMFLOAT4 color;
        };
        ByteAddressBuffer* mVertexBuffer = nullptr;
    };
}

CREATE_APPLICATION(sandbox::Application);
