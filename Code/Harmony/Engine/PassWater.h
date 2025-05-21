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
                graphics::dx12::BufferView inVertexBuffer;
                graphics::dx12::DescriptorHandle inRTV;
            };
            void render(graphics::GraphicContext& context, InputOutputs& inputOutputs);

            ~Water() = default;

        private:
            graphics::dx12::GraphicsPipelineState initialize(graphics::GraphicContext& context);

        private:
            graphics::dx12::GraphicsPipelineState mTrianglePSO;
        };
    }
}
