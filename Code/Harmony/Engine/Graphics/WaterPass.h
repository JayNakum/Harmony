#pragma once

#include "common.h"
#include "Log.h"

#include "CommandContext.h"
#include "ShaderInput.h"

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
                unsigned int width, height;
            };

            struct Outputs
            {
                graphics::dx12::GPUTexture& outputRender;
            };

            Water(graphics::GraphicContext& context);

            void freeUploadResources();

            Outputs render(graphics::GraphicContext& context, Inputs& passInputs);

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

            struct WhirlpoolParams
            {
                vec2f center = vec2f(0.0f);
                float swirlRadius = 0.0f;
                float wrappedSwirlAngle = 0.0f;

                float sinkRadius = 0.0f;
                float sinkDepth = 0.0f;
                const uint32_t _whirlpoolParamsPaddingFinal[2] = { 0x1deadb0b, 0x1deadb0b };
            };
            double mCurrentSwirlAngle = 0.0;

            struct alignas(256) WaterParams
            {
                static constexpr unsigned int NUM_WAVES = 3;
                WaveParams waveParams[NUM_WAVES];

                static constexpr unsigned int NUM_WHIRLPOOLS = 1;
                WhirlpoolParams whirlpoolParams[NUM_WHIRLPOOLS];

                float wavesMaxHeight = 0.0f;
                const uint32_t _waterParamsPadding0 = 0x1deadb0b;
                const uint32_t _waterParamsPadding1 = 0x1deadb0b;
                const uint32_t _waterParamsPadding2 = 0x1deadb0b;
            };
            static_assert(sizeof(WaterParams) % 256 == 0, "Must be aligned");

            void setWaterParams(graphics::GraphicContext& context, graphics::ShaderInput& waterSI, const FrameConstants& frame, const unsigned int paramsLayoutIndex);

        private:
            graphics::dx12::GraphicsPipelineState initialize(graphics::GraphicContext& context);

            void prepareRenderItem(graphics::GraphicContext& context);

        private:
            graphics::dx12::RenderTarget::Description mRTDescription;
            graphics::dx12::RenderTarget* mRenderTarget;
            graphics::dx12::DescriptorHandle mRTV;

            std::vector<graphics::ShaderInput::Constants> mConstantsLayout;
            std::vector<graphics::ShaderInput::Parameter> mParametersLayout;
            graphics::dx12::GraphicsPipelineState mWaterPSO;

            graphics::dx12::UploadBuffer* mPassCB;
            graphics::dx12::UploadBuffer* mWaterParamsCB;

            RenderItem* mWaterRI;

        private:
            PassConstants mPass;
            vec3f mEyePos = { 5.0f, 10.0f, -25.0f };
            mat4f mView;
            mat4f mProjection;
        };
    }
}
