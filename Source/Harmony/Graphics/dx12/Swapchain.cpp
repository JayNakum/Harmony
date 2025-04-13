#include "Swapchain.h"

#include "Core.h"

#include "types/DefaultFormats.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        Swapchain::Swapchain(const Description& createDescription, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles)
            : mSwapChain3(nullptr), mBackBufferDescription(createDescription.renderTargetDescription),
            mBackBuffers{createDescription.renderTargetDescription, createDescription.renderTargetDescription , createDescription.renderTargetDescription }
        {
            Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
            {
                HRESULT hResult = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
                HASSERT_HR(hResult, "Failed to create dxgi factory.");
            }

            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.Width = createDescription.width;
            swapChainDesc.Height = createDescription.height;
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
                GfxCommandListManager().getGraphicsQueue().get(),
                createDescription.windowHandle,
                &swapChainDesc,
                &fsSwapChainDesc,
                nullptr,
                &swapchain1);

            HASSERT_HR(hResult, "Failed to create swapchain %s.", createDescription.name);
            HRESULT hr = swapchain1.As(&mSwapChain3);
            HASSERT_HR(hr, "Failed to cast SwapChain1 to SwapChain3.");

            resize(createDescription.width, createDescription.height, rtvHandles);
        }

        void Swapchain::resize(unsigned int width, unsigned int height, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles)
        {
            for (size_t i = 0; i < BACK_BUFFER_COUNT; ++i)
            {
                mBackBuffers[i].getTexture().reset();
            }

            HRESULT hResult = mSwapChain3->ResizeBuffers(
                BACK_BUFFER_COUNT,
                width, height,
                toDXGIFormat(mBackBufferDescription.textureDescription.format),
                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
            );
            HASSERT_HR(hResult, "Failed to resize swapchain.");
            
            for (size_t i = 0; i < BACK_BUFFER_COUNT; ++i)
            {
                Microsoft::WRL::ComPtr<ID3D12Resource> backBufferResource;
                mSwapChain3->GetBuffer(i, IID_PPV_ARGS(&backBufferResource));

                GPUResource backBuffer(backBufferResource.Get(), GPUResourceState::Present);

                std::string name = "swapchain buffer" + std::to_string(i);
                setName(backBuffer.get(), utility::toWideString(name).c_str());

                mBackBuffers[i].createFromSwapchain(rtvHandles[i], backBuffer);
            }
        }

        void Swapchain::present(unsigned int interval)
        {
            HRESULT hResult = mSwapChain3->Present(interval, 0);

            HASSERT_HR(hResult, "Failed to present swapchain.");

            mCurrentBuffer = mSwapChain3->GetCurrentBackBufferIndex();
        }

        Swapchain::~Swapchain()
        {
            mSwapChain3 = nullptr;
        }
    }
}
