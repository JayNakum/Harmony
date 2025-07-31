#pragma once

#include "PassCore.h"

namespace engine
{
    class Noise : DispatchPass
    {
    public:
        Noise(graphics::Device& device, ResourceManager& resourceManager);

        void copyDataToGPU(graphics::ComputeContext& context, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override;

        void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) override;
        void update(const double timeStep) override;

        void getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager) override;
        void dispatch(graphics::ComputeContext& context, const FrameConstants& frameConstants) override;

        ~Noise() = default;

    private:
        graphics::ComputePipelineState createPSO(graphics::Device& device);

    private:
        const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
        graphics::ResourceTable mShaderResources;
        graphics::ShaderInput mNoiseSI;

        graphics::ComputePipelineState mNoisePSO;
    };
}
