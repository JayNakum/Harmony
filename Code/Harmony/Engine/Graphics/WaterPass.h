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
                PassConstants pass;
            };

            struct Outputs
            {
                graphics::dx12::GPUTexture& outputRender;
            };

            Water(graphics::GraphicContext& context);

            Outputs render(graphics::GraphicContext& context, Inputs& passInputs);

            ~Water();

        private:
            graphics::dx12::GraphicsPipelineState initialize(graphics::GraphicContext& context);

        private:
            graphics::dx12::RenderTarget::Description mRTDescription;
            graphics::dx12::RenderTarget* mRenderTarget;
            graphics::dx12::DescriptorHandle mRTV;

            graphics::dx12::GraphicsPipelineState mTrianglePSO;

            graphics::dx12::UploadBuffer* mIntermediateVertexBuffer;
            graphics::dx12::ByteAddressBuffer* mVertexBuffer;
        };
    }
}
