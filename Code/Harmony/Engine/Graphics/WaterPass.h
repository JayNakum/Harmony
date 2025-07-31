#pragma once

#include "PassCore.h"

namespace engine
{
    class Water : RenderPass
    {
    public:
        Water(graphics::Device& device, ResourceManager& resourceManager);
        void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
        void freeGPUUploadData(ResourceManager& resourceManager) override;

        void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) override;
        void update(const double timeStep) override;

        void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
        void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) override;

        ~Water() = default;

    private:
        void updateCamera();

        struct GerstnerWaveParams
        {
            vec2f direction = vec2f(0.0f);

            float amplitude = 0.0f;
            float steepness = 0.0f;
            float wavelength = 0.0f;

            const uint32_t _gerstnerWaveParamsPadding[3] = { 0x1deadb0b, 0x1deadb0b, 0x1deadb0b };
        };

        struct FBMParams
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

        struct WaterParams
        {
            static constexpr unsigned int NUM_WAVES = 3;
            GerstnerWaveParams gerstnerWaveParams[NUM_WAVES];
            FBMParams fbmParams;

            vec4f deepColor;
            vec4f shallowColor;
            float shallowFade;

            float wavesTotalAmplitude = 0.0f;

            const uint32_t _waterParamsPaddingFinal[214] = { 0x1deadb0b };
        };
        static_assert(sizeof(WaterParams) % 256 == 0, "Must be aligned");

    private:
        graphics::GraphicsPipelineState createPSO(graphics::Device& device);

    private:
        const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
        graphics::ResourceTable mShaderResources;
        graphics::ShaderInput mWaterSI;

        graphics::GraphicsPipelineState mWaterPSO;

        graphics::DescriptorHandle mRTV;
        graphics::HeapWithAllocator::ResourceDescription mRenderTargetDescription;

        graphics::Buffer* mCommonPassConstantsBuffer;

        RenderItem mWaterRI;

    private:
        vec3f mEyePos = { 5.0f, 10.0f, -25.0f };
        mat4f mView;
        mat4f mProjection;
        vec2f mNearFarZ;
    };
}
