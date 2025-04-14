#include "Entrypoint.h"

#include "dx12.h"
#include "Core.h"
#include "DescriptorHeapWithAllocator.h"

namespace sandbox
{
    using namespace graphics;
    using namespace dx12;

    class Application : public harmony::Application
    {
        static constexpr unsigned int CLIENT_WIDTH = 1280;
        static constexpr unsigned int CLIENT_HEIGHT = 720;

    public:
        Application()
            : mWindow({L"Sandbox", CLIENT_WIDTH, CLIENT_HEIGHT})
        {
        }

        void initialize() override
        {
            GfxCommandListManager().createNewCommandList(CommandListType::Direct, mCommandList, mCommandAllocator);

            {// ShaderView descriptor heap
                DescriptorHeapWithAllocator::Description createDescription{};
                createDescription.name = "main shader views descriptor heap";
                createDescription.type = DescriptorHeapType::ShaderViews;
                createDescription.maxDescriptors = 32;
                createDescription.flag = DescriptorHeapFlags::GPUVisible;
                mShaderViewsHeap = new DescriptorHeapWithAllocator(createDescription);
            }
            {// RenderTargetView descriptor heap
                DescriptorHeapWithAllocator::Description createDescription{};
                createDescription.name = "main render target views descriptor heap";
                createDescription.type = DescriptorHeapType::RenderTarget;
                createDescription.maxDescriptors = 8;
                createDescription.flag = DescriptorHeapFlags::CPUVisible;
                mRenderTargetViewsHeap = new DescriptorHeapWithAllocator(createDescription);
            }

            {// Swapchain
                Swapchain::Description createDescription{};
                createDescription.name = "main swapchain";
                createDescription.windowHandle = mWindow.getHandle();

                createDescription.renderTargetDescription.type = RenderTarget::Type::Color;
                createDescription.renderTargetDescription.textureDescription.width = CLIENT_WIDTH;
                createDescription.renderTargetDescription.textureDescription.height = CLIENT_HEIGHT;
                createDescription.renderTargetDescription.textureDescription.sampleCount = 1;
                createDescription.renderTargetDescription.textureDescription.sampleQuality = 0;
                createDescription.renderTargetDescription.textureDescription.format = DefaultFormats::R8G8B8A8_UNORM;
                createDescription.renderTargetDescription.textureDescription.resourceFlags = GPUResourceFlags::AllowRenderTarget;
                createDescription.renderTargetDescription.textureDescription.dimension = GPUResourceDimension::Texture2D;

                DescriptorHandle rtvHandles = mRenderTargetViewsHeap->allocate(3);
                DescriptorHandle rtv1 = rtvHandles + 0 * mRenderTargetViewsHeap->getDescriptorSize();
                DescriptorHandle rtv2 = rtvHandles + 1 * mRenderTargetViewsHeap->getDescriptorSize();
                DescriptorHandle rtv3 = rtvHandles + 2 * mRenderTargetViewsHeap->getDescriptorSize();

                mSwapchain = new Swapchain(createDescription, {rtv1, rtv2, rtv3});
            }
        }

        void run() override
        {
            while (mWindow.update())
            {
            }
        }

        void deinitialize() override
        {
            GfxCommandListManager().getGraphicsQueue().waitForIdle();
        }

        ~Application()
        {
        }

    private:
        windows::Window mWindow;

        DescriptorHeapWithAllocator* mShaderViewsHeap = nullptr;
        DescriptorHeapWithAllocator* mRenderTargetViewsHeap = nullptr;

        Swapchain* mSwapchain = nullptr;

        CommandAllocator* mCommandAllocator = nullptr;
        CommandList* mCommandList = nullptr;
    };
}

CREATE_APPLICATION(sandbox::Application);
