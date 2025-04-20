#pragma once

#include "common.h"

#include "dx12/Device.h"
#include "CommandListManager.h"
#include "dx12/Swapchain.h"
#include "CommandContext.h"
#include "CommandContextPool.h"
#include "DescriptorHeapWithAllocator.h"

namespace engine
{
    class Renderer
    {
    public:
        Renderer(const HWND handle, const unsigned int clientWidth, const unsigned int clientHeight);

        inline graphics::GraphicContext& aquireGraphicContext(const std::string& name)
        {
            graphics::GraphicContext& context = mCommandContextPool.requestGraphicContext(mCommandListManager);
            context.setName(name);
            return context;
        }

        inline void discardGraphicContext(graphics::GraphicContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }

        void render();
        void present();

        ~Renderer();

    private:
        graphics::dx12::Device mDevice;
        graphics::CommandListManager mCommandListManager;
        graphics::dx12::RenderTarget::Description mRenderTargetDescription;

        graphics::DescriptorHeapWithAllocator mRTVDescriptorHeapCPU;
        graphics::dx12::Swapchain mSwapchain;
        graphics::CommandContextPool mCommandContextPool;

        graphics::DescriptorHeapWithAllocator mViewDescriptorHeapGPU;
        graphics::DescriptorHeapWithAllocator mViewDescriptorHeapCPU;
    };
}
