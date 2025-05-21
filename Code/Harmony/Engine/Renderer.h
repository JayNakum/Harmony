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
#include "PassWater.h"
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

        void resize(const unsigned int width, const unsigned int height);
        void render();

        ~Renderer();

    private:
        graphics::dx12::Device mDevice;
        graphics::dx12::CommandQueue mGraphicsQueue;

        graphics::CommandContextPool mCommandContextPool;

        graphics::DynamicDescriptorHeap mShaderViewsHeap;

        graphics::DescriptorHeapWithAllocator mRTVDescriptorHeapCPU;
        graphics::DescriptorHeapWithAllocator::Page mRTVPage;

        graphics::dx12::Swapchain mSwapchain;
        unsigned int mClientWidth;
        unsigned int mClientHeight;

        graphics::dx12::RenderTarget::Description mRenderTargetDescription;

    private:
        graphics::dx12::UploadBuffer* intermediateBuffer;
        graphics::dx12::ByteAddressBuffer* vertexBuffer;
        graphics::dx12::RenderTarget* waterRT;
        graphics::dx12::DescriptorHandle waterRTV;
        renderPass::Water* mWaterPass;
        renderPass::Present* mPresentPass;
    };
}
