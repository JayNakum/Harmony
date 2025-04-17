#include "Swapchain.h"

#include "types/DefaultFormats.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        Swapchain::Swapchain(Device& device, CommandQueue& queue, const Description& createDescription, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles)
            : mSwapChain(nullptr), mBackBufferDescription(createDescription.renderTargetDescription),
            mBackBuffers{createDescription.renderTargetDescription, createDescription.renderTargetDescription , createDescription.renderTargetDescription }
        {
            Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
            {
                HRESULT hResult = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
                HASSERT(SUCCEEDED(hResult), "Failed to create dxgi factory.\n%s", utility::translateErrorCode(hResult));
            }

            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.Width = (UINT)createDescription.renderTargetDescription.textureDescription.width;
            swapChainDesc.Height = (UINT)createDescription.renderTargetDescription.textureDescription.height;
            swapChainDesc.Format = toDXGIFormat(createDescription.renderTargetDescription.textureDescription.format);
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = BACK_BUFFER_COUNT;
            swapChainDesc.SampleDesc.Count = createDescription.renderTargetDescription.textureDescription.sampleCount;
            swapChainDesc.SampleDesc.Quality = createDescription.renderTargetDescription.textureDescription.sampleQuality;
            swapChainDesc.Scaling = DXGI_SCALING_NONE;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
            fsSwapChainDesc.Windowed = TRUE;

            Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain1;
            HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(
                queue.get(),
                createDescription.windowHandle,
                &swapChainDesc,
                &fsSwapChainDesc,
                nullptr,
                &swapchain1);
            HASSERT(SUCCEEDED(hResult), "Failed to create swapchain %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            HRESULT hr = swapchain1.As(&mSwapChain); // A swapchain3 is needed for GetCurrentBackBufferIndex()
            HASSERT(SUCCEEDED(hr), "Failed to cast SwapChain1 to SwapChain3.\n%s", utility::translateErrorCode(hResult));

            resize(device, (unsigned int)createDescription.renderTargetDescription.textureDescription.width, (unsigned int)createDescription.renderTargetDescription.textureDescription.height, rtvHandles);
        }

        void Swapchain::resize(Device& device, const unsigned int width, const unsigned int height, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles)
        {
            // Reset the render target texture.
            for (size_t i = 0; i < BACK_BUFFER_COUNT; ++i)
            {
                mBackBuffers[i].getTexture().reset();
            }

            HRESULT hResult = mSwapChain->ResizeBuffers(
                BACK_BUFFER_COUNT,
                width, height,
                toDXGIFormat(mBackBufferDescription.textureDescription.format),
                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
            );
            HASSERT(SUCCEEDED(hResult), "Failed to resize swapchain.\n%s", utility::translateErrorCode(hResult));

            // Re-create rtvHandles for each back buffer.
            for (unsigned int i = 0; i < BACK_BUFFER_COUNT; ++i)
            {
                Microsoft::WRL::ComPtr<ID3D12Resource> backBufferResource;
                mSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferResource));

                GPUResource backBuffer(backBufferResource.Get(), GPUResourceState::Present); // Creating a GPUResource because createFromSwapchain() cannot work with ID3D12Resource

                std::string name = "swapchain buffer " + std::to_string(i);
                setName(backBuffer.get(), utility::toWideString(name).c_str());

                mBackBuffers[i].createFromSwapchain(device, rtvHandles[i], backBuffer);
            }
        }

        void Swapchain::present(const unsigned int interval)
        {
            HRESULT hResult = mSwapChain->Present(interval, 0);

            HASSERT(SUCCEEDED(hResult), "Failed to present swapchain.\n%s", utility::translateErrorCode(hResult));

            mCurrentBuffer = mSwapChain->GetCurrentBackBufferIndex();
        }

        Swapchain::~Swapchain()
        {
            mSwapChain = nullptr;
        }
    }
}
