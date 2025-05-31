#pragma once

#include "CommandContext.h"
#include "CommonConstants.h"

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

        private:
            graphics::dx12::RenderTarget::Description mRTDescription;
            graphics::dx12::RenderTarget* mRenderTarget;
            graphics::dx12::DescriptorHandle mRTV;

            graphics::dx12::GraphicsPipelineState mWaterPSO;

            graphics::dx12::UploadBuffer* mIntermediateVertexBuffer;
            graphics::dx12::ByteAddressBuffer* mVertexBuffer;
        };
    }
}
