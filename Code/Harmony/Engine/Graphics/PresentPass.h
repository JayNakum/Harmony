#pragma once

#include "common.h"
#include "Log.h"

#include "RenderPass.h"

namespace engine
{
    namespace renderPass
    {
        class Present : RenderPass
        {
        public:
            Present(graphics::dx12::Device& device, ResourceManager& resourceManager);

            void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
            void freeGPUUploadData(ResourceManager& resourceManager) override {}

            void resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager, const bool isHDRSupported);
            void update() override {}

            void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
            void render(graphics::GraphicContext& context, const FrameConstants& frameConstants, graphics::dx12::DescriptorHandle swapchainRTV);

            ~Present() = default;

        private:
            graphics::dx12::GraphicsPipelineState createPSO(graphics::dx12::Device& device);
            void resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager) override {}
            void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) override {};

        private:
            const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
            graphics::ResourceTable mShaderResources;
            graphics::ShaderInput mPresentSI;
            graphics::dx12::GraphicsPipelineState mPresentPSO;
            unsigned int mSwapchainWidth;
            unsigned int mSwapchainHeight;
        };
    }
}
