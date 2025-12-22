#include "Swapchain.h"

#include "types/DefaultFormats.h"

namespace graphics
{
    Swapchain::Swapchain(Device& device, CommandQueue& queue, const Description& createDescription, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles)
        : mSwapChain(nullptr),
        mRenderTargetDescription{
            "Swapchain_BackBuffer",
            createDescription.width,
            createDescription.height,
            1, 1,
            GPUResourceDimension::Texture2D,
            DefaultFormats::R8G8B8A8_UNORM,
            GPUResourceFlags::AllowRenderTarget
        },
        mBackBuffers{
            Texture(mRenderTargetDescription),
            Texture(mRenderTargetDescription),
            Texture(mRenderTargetDescription)
        },
        mRtvHandles(rtvHandles),
        mTearingSupport(false), mHDRSupport(false)
    {
        Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
        {// Create dxgiFactory
            HRESULT hResult = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
            HASSERT(SUCCEEDED(hResult), "Failed to create dxgi factory.\n%s", utility::translateErrorCode(hResult).c_str());
        }

        {// Check for tearing support
            BOOL allowTearing = FALSE;
            HRESULT hResult = dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
            mTearingSupport = SUCCEEDED(hResult) && allowTearing;
        }

        {// Create swapchain
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.Width = static_cast<UINT>(createDescription.width);
            swapChainDesc.Height = static_cast<UINT>(createDescription.height);
            swapChainDesc.Format = dx12::toDXGIFormat(DefaultFormats::R8G8B8A8_UNORM);
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = BACK_BUFFER_COUNT;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.Scaling = DXGI_SCALING_NONE;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | (mTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

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
            HASSERT(SUCCEEDED(hResult), "Failed to create swapchain %s.\n%s", createDescription.name.c_str(), utility::translateErrorCode(hResult).c_str());

            hResult = swapchain1.As(&mSwapChain); // A swapchain3 is needed for GetCurrentBackBufferIndex()
            HASSERT(SUCCEEDED(hResult), "Failed to cast SwapChain1 to SwapChain3.\n%s", utility::translateErrorCode(hResult).c_str());
        }

        resize(device, static_cast<unsigned int>(createDescription.width), static_cast<unsigned int>(createDescription.height), rtvHandles);
    }

    void Swapchain::resize(Device& device, const unsigned int width, const unsigned int height, std::array<DescriptorHandle, BACK_BUFFER_COUNT> rtvHandles)
    {
        checkHDRSupport();

        // Reset the render target texture.
        for (size_t i = 0; i < BACK_BUFFER_COUNT; ++i)
        {
            mBackBuffers[i].reset();
            mRtvHandles[i] = rtvHandles[i];
        }

        HRESULT hResult = mSwapChain->ResizeBuffers(
            BACK_BUFFER_COUNT,
            width, height,
            dx12::toDXGIFormat(mHDRSupport ? DefaultFormats::R10G10B10A2_UNORM : DefaultFormats::R8G8B8A8_UNORM),
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | (mTearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0)
        );
        HASSERT(SUCCEEDED(hResult), "Failed to resize swapchain.\n%s", utility::translateErrorCode(hResult).c_str());

        // Re-create rtvHandles for each back buffer.
        for (unsigned int i = 0; i < BACK_BUFFER_COUNT; ++i)
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> backBufferResource;
            HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferResource));
            HASSERT(SUCCEEDED(hr), "Failed to get swapchain back buffer.\n%s", utility::translateErrorCode(hr).c_str());

            mBackBuffers[i].setResource(backBufferResource.Get());
            HASSERT(mBackBuffers[i].get() != nullptr, "mBackBuffers is null.");

            device->CreateRenderTargetView(mBackBuffers[i].get(), nullptr, rtvHandles[i]);
        }
    }

    void Swapchain::present(const unsigned int interval)
    {
        UINT presentInterval = (!mTearingSupport) ? interval : 0;
        UINT presentFlags = (mTearingSupport) ? DXGI_PRESENT_ALLOW_TEARING : 0;
        HRESULT hResult = mSwapChain->Present(presentInterval, presentFlags);
        HASSERT(SUCCEEDED(hResult), "Failed to present swapchain.\n%s", utility::translateErrorCode(hResult).c_str());
    }

    void Swapchain::checkHDRSupport()
    {
        Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain4;
        HRESULT hResult = mSwapChain.As(&swapChain4);
        HASSERT_SOFT(SUCCEEDED(hResult), "Failed to cast SwapChain3 to SwapChain4.\n%s", utility::translateErrorCode(hResult).c_str());

        Microsoft::WRL::ComPtr<IDXGIOutput> output;
        Microsoft::WRL::ComPtr<IDXGIOutput6> output6;
        DXGI_OUTPUT_DESC1 outputDesc;
        UINT colorSpaceSupport;

        // Query support for ST.2084 on the display and set the color space accordingly
        if (SUCCEEDED(swapChain4->GetContainingOutput(&output)) &&
            SUCCEEDED(output.As(&output6)) &&
            SUCCEEDED(output6->GetDesc1(&outputDesc)) &&
            outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 &&
            SUCCEEDED(swapChain4->CheckColorSpaceSupport(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020, &colorSpaceSupport)) &&
            (colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) &&
            SUCCEEDED(swapChain4->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)))
        {
            mHDRSupport = true;
        }
    }
}
