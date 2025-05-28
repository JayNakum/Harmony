#pragma once

#include "CommandContext.h"

namespace engine
{
    namespace renderPass
    {
        class Water
        {
        public:
            Water(graphics::GraphicContext& context);

            struct InputOutputs
            {
                unsigned int width, height;

                graphics::dx12::DescriptorHandle inRTV;
            };
            void render(graphics::GraphicContext& context, InputOutputs& inputOutputs);

            ~Water();

        private:
            graphics::dx12::GraphicsPipelineState initialize(graphics::GraphicContext& context);

        private:
            graphics::dx12::GraphicsPipelineState mTrianglePSO;

            graphics::dx12::UploadBuffer* mIntermediateVertexBuffer;
            graphics::dx12::ByteAddressBuffer* mVertexBuffer;
        };
    }
}
