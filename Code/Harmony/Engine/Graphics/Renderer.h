#pragma once

#include "common.h"
#include "Log.h"

#include "dx12/Device.h"
#include "dx12/CommandQueue.h"
#include "dx12/Swapchain.h"

#include "CommandContext.h"
#include "CommandContextPool.h"
#include "ResourceManager.h"


#include "PassCore.h"
// Render Passes
#include "DebugUI.h"
#include "WaterPass.h"
#include "PresentPass.h"
// Dispatch Passes
#include "NoisePass.h"

#include "Maths.h"

namespace engine
{
    /*
    * Core Graphics Renderer
    * Responsible for managing render and dispatch passes.
    */
    class Renderer
    {
    public:
        Renderer(const HWND windowHandle);

        inline graphics::ComputeContext& aquireComputeContext(const std::string& name)
        {
            graphics::ComputeContext& context = mCommandContextPool.requestComputeContext(mDevice);
            context.setName(name);
            context.reset(mComputeQueue.incrementFence());
            context.bindDescriptorHeap(mResourceManager.getDescriptorHeap(graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU));
            return context;
        }
        inline graphics::GraphicContext& aquireGraphicContext(const std::string& name)
        {
            graphics::GraphicContext& context = mCommandContextPool.requestGraphicContext(mDevice);
            context.setName(name);
            context.reset(mGraphicsQueue.incrementFence());
            context.bindDescriptorHeap(mResourceManager.getDescriptorHeap(graphics::DescriptorHeapWithAllocator::DescriptorsCategory::StaticGPU));
            return context;
        }
        inline graphics::CopyContext& aquireCopyContext(const std::string& name)
        {
            graphics::CopyContext& context = mCommandContextPool.requestCopyContext(mDevice);
            context.setName(name);
            context.reset(mCopyQueue.incrementFence());
            return context;
        }

        inline void discardContext(graphics::CopyContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }
        inline void discardContext(graphics::ComputeContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }
        inline void discardContext(graphics::GraphicContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }

        void resize(const unsigned int width, const unsigned int height);

        void update(const double timeStep);

        void render(const FrameConstants frame);

        ~Renderer();

    private:
        graphics::Device mDevice;
        graphics::CommandQueue mCopyQueue;
        graphics::CommandQueue mComputeQueue;
        graphics::CommandQueue mGraphicsQueue;
        ResourceManager mResourceManager;

        graphics::CommandContextPool mCommandContextPool;

        graphics::Swapchain mSwapchain;
        unsigned int mClientWidth, mClientHeight;

    private:
        DebugUI mDebugUI;
        PresentPass mPresentPass;
        WaterPass mWaterPass;

    private:
        NoisePass mNoisePass;
    };
}
