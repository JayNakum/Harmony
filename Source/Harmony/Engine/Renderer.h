#pragma once

#include "common.h"

#include "dx12/Device.h"
#include "CommandListManager.h"
#include "CommandContext.h"
#include "CommandContextPool.h"

#include "DescriptorHeapWithAllocator.h"
#include "DynamicDescriptorHeap.h"

#include "dx12/Swapchain.h"

namespace engine
{
    class Renderer
    {
    public:
        Renderer(const HWND handle, const unsigned int clientWidth, const unsigned int clientHeight);

        inline graphics::GraphicContext& aquireGraphicContext(const std::string& name)
        {
            graphics::CommandListManager::CommandObjects newObjects = mCommandListManager.createNewCommandList(graphics::CommandListType::Direct);

            graphics::GraphicContext& context = mCommandContextPool.requestGraphicContext(newObjects.commandList);
            context.setName(name);
            context.reset(newObjects.commandAllocator);
            context.setDevice(mDevice);
            context.setShaderViewHeap(mShaderViewsHeap);
            return context;
        }

        inline void discardGraphicContext(graphics::GraphicContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }

        /*void render();
        void present();*/

        ~Renderer();

    private:
        graphics::dx12::Device mDevice;
        graphics::CommandListManager mCommandListManager;
        graphics::CommandContextPool mCommandContextPool;

        graphics::DynamicDescriptorHeap mShaderViewsHeap;

        graphics::dx12::RenderTarget::Description mRenderTargetDescription;
        graphics::DescriptorHeapWithAllocator mRTVDescriptorHeapCPU;
        graphics::dx12::Swapchain mSwapchain;
    };
}
