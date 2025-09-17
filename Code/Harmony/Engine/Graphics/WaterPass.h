#pragma once

#include "PassCore.h"

namespace engine
{
    class WaterPassData : public PassData
    {
    public:
        struct alignas(16) GerstnerWaveParams
        {
            vec2f direction = vec2f(0.0f);

            float amplitude = 0.0f;
            float steepness = 0.0f;
            float wavelength = 0.0f;
        };

        struct alignas(16) FBMParams
        {
            unsigned int octaves;
            float lacunarity;
            float gain;
            float noiseScale;

            float noiseSpeed;
            float rippleAmp;

            float lowFreq;
            float highFreq;
        };

        struct alignas(16) WaterPassParams
        {
            static constexpr unsigned int NUM_WAVES = 3;
            std::array<GerstnerWaveParams, NUM_WAVES> gerstnerWaveParams;
            FBMParams fbmParams;

            vec4f deepColor;
            vec4f shallowColor;
            float shallowFade;

            float wavesTotalAmplitude = 0.0f;
        };

    public:
        WaterPassData();

        inline const WaterPassParams& getWaterParams() const { return mWaterPassParams; }
        inline const GeometryGenerator::MeshData& getWaterMesh() const { return mWaterMesh; }
        inline GeometryGenerator::MeshData& getWaterMesh() { return mWaterMesh; }

        void renderDebugUI() override;
        void reset() override;

        ~WaterPassData() override = default;

    private:
        WaterPassParams mWaterPassParams;
        GeometryGenerator::MeshData mWaterMesh;
        GeometryGenerator mGeoGen;
    };

    class WaterPass : RenderPass
    {
    public:
        WaterPass(graphics::Device& device, ResourceManager& resourceManager);
        void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override;

        void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) override;
        void update(const double timeStep) override;

        void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
        void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) override;

        ~WaterPass() = default;

    private:
        void updateCamera();

    private:
        graphics::GraphicsPipelineState createPSO(graphics::Device& device);

    private:
        WaterPassData mWaterPassData;
        const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
        graphics::ResourceTable mShaderResources;
        graphics::ShaderInput mWaterPassSI;

        graphics::GraphicsPipelineState mWaterPassPSO;

        graphics::DescriptorHandle mRTV;
        graphics::HeapWithAllocator::ResourceDescription mRenderTargetDescription;

        graphics::Buffer* mCommonPassConstantsBuffer;

    private:
        RenderItem mWaterRI;
    };
}
