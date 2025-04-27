#include "Renderer.h"

namespace engine
{
    Renderer::Renderer(const HWND handle, unsigned int clientWidth, unsigned int clientHeight)
        : mDevice(),
        mGraphicsQueue(mDevice, {"global graphics queue", graphics::CommandListType::Direct}),
        mCommandContextPool(),
        mShaderViewsHeap(mDevice),
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
        mRTVPage(mRTVDescriptorHeapCPU.allocatePage()),
        mSwapchain(
            mDevice, mGraphicsQueue,
            {
                "swapchain",
                handle,
                mRenderTargetDescription
            },
            {
                mRTVPage.allocate(1),
                mRTVPage.allocate(1),
                mRTVPage.allocate(1)
            }
        )
    {}

    Renderer::~Renderer()
    {
    }

}

