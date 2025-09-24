#pragma once

#include "PassCore.h"

namespace engine
{
    class NoisePassData : public PassData
    {
    public:
        NoisePassData();

        virtual void renderDebugUI();
        void reset();

        inline const float getWaterNoiseTextureWidth() const { return mWaterNoiseTextureWidth; }
        inline const float getWaterNoiseTextureHeight() const { return mWaterNoiseTextureHeight; }
        inline const float getWaterNoiseScale() const { return mWaterNoiseScale; }

        ~NoisePassData() override = default;

    private:
        const float mWaterNoiseTextureWidth;
        const float mWaterNoiseTextureHeight;
        float mWaterNoiseScale;
    };

    class NoisePass : DispatchPass
    {
    public:
        NoisePass(graphics::Device& device, ResourceManager& resourceManager);

        void copyDataToGPU(graphics::CopyContext& context, ResourceManager& resourceManager) override {}
        void freeGPUUploadData(ResourceManager& resourceManager) override {}

        void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) override {}
        void update(const double timeStep) override {}

        void getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager) override;
        void dispatch(graphics::ComputeContext& context, const FrameConstants& frameConstants) override;

        ~NoisePass() = default;

    private:
        graphics::ComputePipelineState createPSO(graphics::Device& device);

    private:
        const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
        graphics::ResourceTable mShaderResources;
        graphics::ShaderInput mNoiseSI;

        graphics::ComputePipelineState mNoisePSO;

    private:
        NoisePassData mNoisePassData;
    };
}
