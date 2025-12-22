#pragma once

#include "PassCore.h"

namespace engine
{
    class NoisePassData : public PassData
    {
    public:
        NoisePassData();

        virtual void debugUIRender() override;
        void reset() override;

        inline float getWaterNoiseTextureWidth() const { return mWaterNoiseTextureWidth; }
        inline float getWaterNoiseTextureHeight() const { return mWaterNoiseTextureHeight; }
        inline float getWaterNoiseScale() const { return mWaterNoiseScale; }

    private:
        const float mWaterNoiseTextureWidth;
        const float mWaterNoiseTextureHeight;
        float mWaterNoiseScale;
    };

    class NoisePass : DispatchPass
    {
    public:
        NoisePass(graphics::Device& device, ResourceManager& resourceManager);

        void copyDataToGPU(graphics::CopyContext&, ResourceManager&) override { mNoiseData.setIsDirty(false); }
        void freeGPUUploadData(ResourceManager&) override {}

        void resize(const unsigned int, const unsigned int, graphics::Device&, ResourceManager&) override {}
        void update(const double) override {}

        void getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager) override;
        void dispatch(graphics::ComputeContext& context, const FrameConstants& frameConstants) override;

    private:
        graphics::ComputePipelineState createPSO(graphics::Device& device);

    private:
        NoisePassData mNoiseData;

        const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
        graphics::ResourceTable mShaderResources;
        graphics::ShaderInput mNoiseSI;

        graphics::ComputePipelineState mNoisePSO;
    };
}
