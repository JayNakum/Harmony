#pragma once

#include "common.h"
#include "Log.h"

#include "dx12/Device.h"
#include "dx12/CommandQueue.h"
#include "dx12/Swapchain.h"

#include "CommandContext.h"
#include "CommandContextPool.h"
#include "ResourceManager.h"

#include "CommonConstants.h"

// Render Passes
#include "WaterPass.h"
#include "PresentPass.h"

#include "Maths.h"

namespace engine
{
    class Renderer
    {
    public:
        Renderer(const HWND windowHandle);

        inline graphics::GraphicContext& aquireGraphicContext(const std::string& name)
        {
            graphics::GraphicContext& context = mCommandContextPool.requestGraphicContext(mDevice);
            context.setName(name);
            context.reset(mGraphicsQueue.incrementFence());
            context.bindDescriptorHeap(mResourceManager.getDescriptorHeap(graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU));
            return context;
        }
        inline graphics::ComputeContext& aquireComputeContext(const std::string& name)
        {
            graphics::ComputeContext& context = mCommandContextPool.requestComputeContext(mDevice);
            context.setName(name);
            context.reset(mComputeQueue.incrementFence());
            context.bindDescriptorHeap(mResourceManager.getDescriptorHeap(graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU));
            return context;
        }

        inline void discardGraphicContext(graphics::GraphicContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }
        inline void discardGraphicContext(graphics::ComputeContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }

        void resize(const unsigned int width, const unsigned int height);

        void update(const double timeStep);

        void render(const FrameConstants frame);

        ~Renderer();

    private:
        graphics::dx12::Device mDevice;
        graphics::dx12::CommandQueue mGraphicsQueue;
        graphics::dx12::CommandQueue mComputeQueue;
        ResourceManager mResourceManager;

        graphics::CommandContextPool mCommandContextPool;

        graphics::dx12::Swapchain mSwapchain;
        unsigned int mClientWidth, mClientHeight;

    private:
        renderPass::Water mWaterPass;
        renderPass::Present mPresentPass;
    };
}
