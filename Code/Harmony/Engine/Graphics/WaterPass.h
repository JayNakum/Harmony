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
            graphics::dx12::GraphicsPipelineState initialize(graphics::GraphicContext& context);

            void prepareRenderItem(graphics::GraphicContext& context);

            void updateCamera();
            void updateConstantBuffers(graphics::GraphicContext& context, const FrameConstants& frame);

        private:
            graphics::dx12::RenderTarget::Description mRTDescription;
            graphics::dx12::RenderTarget* mRenderTarget;
            graphics::dx12::DescriptorHandle mRTV;

            std::vector<graphics::ShaderInput::Parameter> mParametersLayout;
            graphics::dx12::GraphicsPipelineState mWaterPSO;

            graphics::dx12::UploadBuffer* mPassCB;

            RenderItem* mWaterRI = nullptr;

        private:
            PassConstants mPass;
            vec3f mEyePos = { 0.0f, 1.5f, -15.0f };
            mat4f mView;
            mat4f mProjection;
        };
    }
}
