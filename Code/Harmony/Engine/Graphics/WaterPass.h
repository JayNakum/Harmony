#pragma once

#include "common.h"
#include "Log.h"

#include "CommandContext.h"
#include "ShaderInput.h"
#include "ResourceManager.h"
#include "CommonConstants.h"
#include "RenderItem.h"

namespace engine
{
    namespace renderPass
    {
        class Water
        {
        public:
            struct Inputs
            {
                FrameConstants frame;
            };

            struct Outputs
            {
                graphics::dx12::Texture& outputRender;
            };

            Water(graphics::dx12::Device& device, ResourceManager& resourceManager);
            void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager);
            void freeGPUUploadData(ResourceManager& resourceManager);

            void resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager);
            void update();

            void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager);
            void render(graphics::GraphicContext& context, FrameConstants& frameConstants);

            ~Water();

        private:
            void updateCamera();
            void setCommonParams(graphics::GraphicContext& context, graphics::ShaderInput& waterSI, const FrameConstants& frame);

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
                float wavesMaxHeight = 0.0f;

                const uint32_t _waveParamsPaddingFinal[231];
            };
            static_assert(sizeof(WaterParams) % 256 == 0, "Must be aligned");

            void setWaterParams(graphics::GraphicContext& context, graphics::ShaderInput& waterSI, const FrameConstants& frame, const unsigned int paramsLayoutIndex);

        private:
            graphics::dx12::GraphicsPipelineState createPSO(graphics::dx12::Device& device);

            void prepareRenderItem(graphics::GraphicContext& context);

        private:
            graphics::dx12::GraphicsPipelineState mWaterPSO;

            const graphics::ResourceTable::Layout mStaticShaderResourceTableLayout;
            graphics::ShaderInput mWaterSI;
            graphics::ResourceTable mShaderResources;

            graphics::dx12::DescriptorHandle mRTV;
            graphics::HeapWithAllocator::ResourceDescription mRenderTargetDescription;

            RenderItem mWaterRI;

        private:
            PassConstants mPass;
            vec3f mEyePos = { 5.0f, 10.0f, -25.0f };
            mat4f mView;
            mat4f mProjection;
        };
    }
}
