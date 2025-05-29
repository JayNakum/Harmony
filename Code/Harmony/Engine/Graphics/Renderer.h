#pragma once

#include "common.h"

#include "dx12/Device.h"
#include "dx12/CommandQueue.h"

#include "dx12/Swapchain.h"

#include "CommandContext.h"
#include "CommandContextPool.h"
#include "DescriptorHeapWithAllocator.h"
#include "DynamicDescriptorHeap.h"

#include "CommonConstants.h"

// Render Passes
#include "WaterPass.h"
#include "PresentPass.h"

#include "Maths.h"

namespace engine
{
    struct RenderItem
    {
        graphics::dx12::BufferView vertexBuffer;
        graphics::dx12::BufferView indexBuffer;
        graphics::PrimitiveTopology primitiveTopology;

        mat4f modelMatrix;
    };

    class Renderer
    {
    public:
        Renderer(const HWND windowHandle);

        inline graphics::GraphicContext& aquireGraphicContext(const std::string& name)
        {
            graphics::GraphicContext& context = mCommandContextPool.requestGraphicContext(mDevice, mShaderViewsHeap, mRTVDescriptorHeapCPU);
            context.setName(name);
            context.reset(mGraphicsQueue.incrementFence());
            return context;
        }

        inline void discardGraphicContext(graphics::GraphicContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }

        void resize(const unsigned int width, const unsigned int height);

        void render(const FrameConstants frame);

        ~Renderer();

    private:
        graphics::dx12::Device mDevice;
        graphics::dx12::CommandQueue mGraphicsQueue;

        graphics::CommandContextPool mCommandContextPool;

        graphics::DynamicDescriptorHeap mShaderViewsHeap;

        graphics::DescriptorHeapWithAllocator mRTVDescriptorHeapCPU;
        graphics::DescriptorHeapWithAllocator::Page mSwapchainRTVPage;

        graphics::dx12::Swapchain mSwapchain;

    private:
        renderPass::Water* mWaterPass;
        renderPass::Present* mPresentPass;
    };
}
