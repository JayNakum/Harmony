#pragma once

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
            struct WaterParams
            {
                static constexpr unsigned int NUM_WAVES = 3;

                float amplitudes[NUM_WAVES];
                vec2f windDirections[NUM_WAVES];
                float steepnesses[NUM_WAVES];
                float wavelengths[NUM_WAVES];
                const uint32_t _waterParamsPadding0[NUM_WAVES] = { 0x1deadb0b, 0x1deadb0b, 0x1deadb0b };
                const uint32_t _waterParamsPadding1[NUM_WAVES] = { 0x1deadb0b, 0x1deadb0b, 0x1deadb0b };
                const uint32_t _waterParamsPadding2[NUM_WAVES] = { 0x1deadb0b, 0x1deadb0b, 0x1deadb0b };
            };

            void setWaterParams(const WaterParams& params, graphics::ShaderInput& waterSI, const unsigned int constantsLayoutIndex = 0) const;

        private:
            graphics::dx12::GraphicsPipelineState initialize(graphics::GraphicContext& context);

            void prepareRenderItem(graphics::GraphicContext& context);

            void updateCamera();
            void updateConstantBuffers(graphics::GraphicContext& context, const FrameConstants& frame);

        private:
            graphics::dx12::RenderTarget::Description mRTDescription;
            graphics::dx12::RenderTarget* mRenderTarget;
            graphics::dx12::DescriptorHandle mRTV;

            std::vector<graphics::ShaderInput::Constants> mConstantsLayout;
            std::vector<graphics::ShaderInput::Parameter> mParametersLayout;
            graphics::dx12::GraphicsPipelineState mWaterPSO;

            graphics::dx12::UploadBuffer* mPassCB;

            RenderItem* mWaterRI = nullptr;

        private:
            PassConstants mPass;
            vec3f mEyePos = { 0.0f, 2.5f, -15.0f };
            mat4f mView;
            mat4f mProjection;
        };
    }
}
