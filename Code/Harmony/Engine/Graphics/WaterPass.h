#pragma once

#include "common.h"
#include "Log.h"

#include "RenderPass.h"

namespace engine
{
    namespace renderPass
    {
        class Water : RenderPass
        {
        public:
            Water(graphics::dx12::Device& device, ResourceManager& resourceManager);
            void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
            void freeGPUUploadData(ResourceManager& resourceManager) override;

            void resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager) override;
            void update(const double timeStep) override;

            void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) override;
            void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) override;

            ~Water() = default;

        private:
            void updateCamera();

            struct WaveParams
            {
                vec2f direction = vec2f(0.0f);
                vec2f center = vec2f(0.0f); // RADIAL WAVES

                float amplitude = 0.0f;
                float steepness = 0.0f;
                float wavelength = 0.0f;
                const uint32_t _waveParamsPadding0 = 0x1deadb0b;
            };

            struct WaterParams
            {
                static constexpr unsigned int NUM_WAVES = 3;
                WaveParams waveParams[NUM_WAVES];
                float wavesTotalAmplitude = 0.0f;

                const uint32_t _waveParamsPaddingFinal[231] = { 0x1deadb0b };
            };
            static_assert(sizeof(WaterParams) % 256 == 0, "Must be aligned");

        private:
            graphics::dx12::GraphicsPipelineState createPSO(graphics::dx12::Device& device);

        private:
            const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
            graphics::ResourceTable mShaderResources;
            graphics::ShaderInput mWaterSI;

            graphics::dx12::GraphicsPipelineState mWaterPSO;

            graphics::dx12::DescriptorHandle mRTV;
            graphics::HeapWithAllocator::ResourceDescription mRenderTargetDescription;

            graphics::dx12::Buffer* mCommonPassConstantsBuffer;

            RenderItem mWaterRI;

        private:
            vec3f mEyePos = { 5.0f, 10.0f, -25.0f };
            mat4f mView;
            mat4f mProjection;
            vec2f mNearFarZ;
        };
    }
}
