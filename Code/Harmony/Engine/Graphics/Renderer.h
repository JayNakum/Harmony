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

    struct GraphicScene
    {
        RenderItem waterRIs;
    };


    struct FrameConstants
    {
        float totalTime = 0.0f;
        float deltaTime = 0.0f;

        uint64_t frameIndex = 0;
    };
    static_assert(sizeof(FrameConstants) % 16 == 0, "Must be aligned");

    struct PassConstants
    {
        mat4f view;
        mat4f invView;

        mat4f projection;
        mat4f invProjection;

        vec3f eyePosition;
        const uint32_t _padding0 = 0x1deadb0b;

        vec2f renderTargetSize;
        vec2f invRenderTargetSize;

        float nearZ = 0.0f;
        float farZ = 0.0f;
        const uint64_t _padding1 = 0x1deadb0b1deadb0b;
    };
    static_assert(sizeof(PassConstants) % 16 == 0, "Must be aligned");

    class ResourceManager
    {
    public:
        ResourceManager(graphics::dx12::Device& device);

        inline const graphics::dx12::DescriptorHandle createRTVs(const unsigned int count = 1)
        {
            if (!mCurrentRTVPage.canAllocate(count))
            {
                mCurrentRTVPage = mRTVDescriptorHeapCPU.allocatePage();
            }

            return mCurrentRTVPage.allocate(count);
        }

        struct CreateInfo
        {
            std::string id;
            union
            {
                graphics::dx12::GPUTexture::Description resourceDescription;
            };
            graphics::GPUResourceStates resourceStates;
        };
        const graphics::dx12::DescriptorHandle create(graphics::CommandContext& context, const CreateInfo& createInfo);

        void initialize(const std::string& id, size_t rowPitchBytes, const void* initialData);

        const graphics::dx12::DescriptorHandle read(const std::string& id, graphics::CommandContext& context, const graphics::dx12::GPUTexture::ShaderResourceViewInfo& view);
        inline const graphics::dx12::DescriptorHandle read(const std::string& id, graphics::CommandContext& context)
        {
            const graphics::dx12::GPUTexture& resource = mTextures.at(id);
            return read(id, context, graphics::dx12::GPUTexture::ShaderResourceViewInfo::getSRVInfo(resource.describeResource()));
        }

        const graphics::dx12::DescriptorHandle write(const std::string& id, graphics::CommandContext& context, const graphics::dx12::GPUTexture::ShaderResourceViewInfo& view);
        inline const graphics::dx12::DescriptorHandle write(const std::string& id, graphics::CommandContext& context)
        {
            const graphics::dx12::GPUTexture& resource = mTextures.at(id);
            return write(id, context, graphics::dx12::GPUTexture::ShaderResourceViewInfo::getSRVInfo(resource.describeResource()));
        }

        const graphics::dx12::DescriptorHandle createRT(const std::string& id, graphics::CommandContext& context, const graphics::dx12::RenderTarget::Description createDescription);

    private:
        static const graphics::GPUResourceStates getResourceStates(const graphics::ShaderVisibility visibility);

    private:
        graphics::dx12::Heap mTexturesHeap;
        std::unordered_map<std::string, graphics::dx12::GPUTexture> mTextures;

    private:
        graphics::DescriptorHeapWithAllocator mRTVDescriptorHeapCPU;
        graphics::DescriptorHeapWithAllocator::Page mCurrentRTVPage;

        graphics::DynamicDescriptorHeap mShaderViewsHeap;
        graphics::DescriptorHeapWithAllocator::Page mCurrentShaderViewsPage;

        std::unordered_map<std::string, graphics::dx12::DescriptorHandle> mDescriptors;
    };

    class Renderer
    {
    public:
        Renderer(const HWND windowHandle);

        inline graphics::GraphicContext& aquireGraphicContext(const std::string& name)
        {
            graphics::GraphicContext& context = mCommandContextPool.requestGraphicContext(mDevice, mShaderViewsHeap);
            context.setName(name);
            context.reset(mGraphicsQueue.incrementFence());
            return context;
        }

        inline void discardGraphicContext(graphics::GraphicContext& usedContext)
        {
            mCommandContextPool.discardContext(usedContext);
        }

        void resize(const unsigned int width, const unsigned int height);

        void setConstants(const FrameConstants frame, const PassConstants pass);
        void render(const GraphicScene& scene);

        ~Renderer();

    private:
        graphics::dx12::Device mDevice;
        graphics::dx12::CommandQueue mGraphicsQueue;

        graphics::CommandContextPool mCommandContextPool;

        ResourceManager mResourceManager;

        graphics::dx12::Swapchain mSwapchain;

    private:
        FrameConstants mFrameConstants;
        PassConstants mPassConstants;

    private:
        renderPass::Water* mWaterPass;
        renderPass::Present* mPresentPass;
    };
}
