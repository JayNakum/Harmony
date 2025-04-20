#include "Renderer.h"

namespace engine
{
    Renderer::Renderer(const HWND handle, unsigned int clientWidth, unsigned int clientHeight)
        : mDevice(),
        mCommandListManager(mDevice),
        mRenderTargetDescription{
            graphics::dx12::RenderTarget::Type::Color,
            "swapchain back buffer",
            clientWidth,
            clientHeight,
            1, 1,
            graphics::GPUResourceDimension::Texture2D,
            graphics::DefaultFormats::R8G8B8A8_UNORM,
            graphics::GPUResourceFlags::AllowRenderTarget,
        },
        mRTVDescriptorHeapCPU(
            mDevice,
            {
                "cpu visible render target view descriptor heap",
                graphics::DescriptorHeapType::ShaderViews,
                6,
                graphics::DescriptorHeapFlags::CPUVisible
            }
        ),
        mSwapchain(
            mDevice, mCommandListManager.getGraphicsQueue(),
            {
                "swapchain",
                handle,
                mRenderTargetDescription
            },
            {
                mRTVDescriptorHeapCPU.allocate(1),
                mRTVDescriptorHeapCPU.allocate(1),
                mRTVDescriptorHeapCPU.allocate(1)
            }
        ),
        mCommandContextPool(),
        mViewDescriptorHeapGPU(
            mDevice,
            {
                "gpu visible shader view descriptor heap",
                graphics::DescriptorHeapType::ShaderViews,
                65535,
                graphics::DescriptorHeapFlags::GPUVisible
            }
        ),
        mViewDescriptorHeapCPU(
            mDevice,
            {
                "cpu visible shader view descriptor heap",
                graphics::DescriptorHeapType::ShaderViews,
                65535,
                graphics::DescriptorHeapFlags::CPUVisible
            }
        )

    {}

    Renderer::~Renderer()
    {
    }

}

