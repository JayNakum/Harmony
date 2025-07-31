#pragma once

#include "PassCore.h"

namespace engine
{
    class PresentPass : RenderPass
    {
    public:
        PresentPass(graphics::Device& device, ResourceManager& resourceManager);
        void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override {}

        void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) override
        {
            HASSERT(false, "Present pass is a special pass that needs additional parameters which are provided in a separate overload.");
        }
        void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager, const bool isHDRSupported);

        void update(const double timeStep) override {}

        void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) override;

        void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) override
        {
            HASSERT(false, "Present pass is a special pass that needs additional parameters which are provided in a separate overload.");
        }
        void render(graphics::GraphicContext& context, const FrameConstants& frameConstants, graphics::DescriptorHandle swapchainRTV);

        ~PresentPass() = default;

    private:
        graphics::GraphicsPipelineState createPSO(graphics::Device& device);

    private:
        const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
        graphics::ResourceTable mShaderResources;
        graphics::ShaderInput mPresentSI;
        graphics::GraphicsPipelineState mPresentPSO;
        unsigned int mSwapchainWidth;
        unsigned int mSwapchainHeight;
    };
}
