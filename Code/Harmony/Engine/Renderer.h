#pragma once

#include "common.h"

#include "dx12/Device.h"
#include "dx12/CommandQueue.h"

#include "dx12/Swapchain.h"

#include "CommandContext.h"
#include "CommandContextPool.h"
#include "DescriptorHeapWithAllocator.h"
#include "DynamicDescriptorHeap.h"

// Render Passes
#include "PassPresent.h"

namespace engine
{
    class Renderer
    {
    public:
        Renderer(const HWND handle, const unsigned int clientWidth, const unsigned int clientHeight);

        inline graphics::GraphicContext& aquireGraphicContext(const std::string& name)
        {
            graphics::GraphicContext& context = mCommandContextPool.requestGraphicContext(mDevice, mShaderViewsHeap);
            context.setName(name);
            context.reset(mGraphicsQueue.getCompletedFence());
            return context;
        }

        inline void discardGraphicContext(graphics::GraphicContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }

        void render();

        ~Renderer();

    private:
        renderPass::Present presentPass;

    private:
        graphics::dx12::Device mDevice;
        graphics::dx12::CommandQueue mGraphicsQueue;

        graphics::CommandContextPool mCommandContextPool;

        graphics::DynamicDescriptorHeap mShaderViewsHeap;

        graphics::dx12::RenderTarget::Description mRenderTargetDescription;
        graphics::DescriptorHeapWithAllocator mRTVDescriptorHeapCPU;
        graphics::DescriptorHeapWithAllocator::Page mRTVPage;
        graphics::dx12::Swapchain mSwapchain;
    };
}
