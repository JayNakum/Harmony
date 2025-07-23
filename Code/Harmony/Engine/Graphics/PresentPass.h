#pragma once

#include "common.h"
#include "Log.h"

#include "CommonConstants.h"
#include "ResourceManager.h"
#include "CommandContext.h"

#include "ShaderInput.h"
#include "RenderItem.h"

namespace engine
{
    namespace renderPass
    {
        class Present
        {
        public:
            Present(graphics::dx12::Device& device, ResourceManager& resourceManager);

            void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager);
            void freeGPUUploadData(ResourceManager& resourceManager) {}

            void resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager, const bool isHDRSupported);
            void update(const double timeStep) {}

            void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager);
            void render(graphics::GraphicContext& context, const FrameConstants& frameConstants, graphics::dx12::DescriptorHandle swapchainRTV);

            ~Present() = default;

        private:
            graphics::dx12::GraphicsPipelineState createPSO(graphics::dx12::Device& device);

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
