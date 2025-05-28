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
        enum class ResourceType
        {
            Buffer,
            Texture,
            RenderTarget
        };

        const std::string createResource(const ResourceType type);

        void readResource(const std::string& id, const graphics::ShaderVisibility visibility);
        void writeResource(const std::string& id, const graphics::ShaderVisibility visibility);

    private:
        std::unordered_map<std::string, graphics::dx12::ByteAddressBuffer> mByteAddressBuffers;
        std::unordered_map<std::string, graphics::dx12::GPUTexture> mTextures;
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

        graphics::DynamicDescriptorHeap mShaderViewsHeap;
        graphics::DescriptorHeapWithAllocator::Page mCurrentShaderViewsPage;

        graphics::DescriptorHeapWithAllocator mRTVDescriptorHeapCPU;
        graphics::DescriptorHeapWithAllocator::Page mCurrentRTVPage;

        graphics::dx12::Swapchain mSwapchain;

    private:
        FrameConstants mFrameConstants;
        PassConstants mPassConstants;

    private:
        graphics::dx12::RenderTarget* waterRT;
        graphics::dx12::DescriptorHandle waterRTV;
        renderPass::Water* mWaterPass;

        graphics::dx12::DescriptorHandle waterSRV;
        renderPass::Present* mPresentPass;
    };
}
