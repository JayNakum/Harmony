#pragma once

#include "CommandContext.h"

namespace engine
{
    namespace renderPass
    {
        class Water
        {
        public:
            Water() = default;

            struct InputOutputs
            {
                graphics::dx12::BufferView inVertexBuffer;
                graphics::dx12::DescriptorHandle inRTV;
            };
            void render(graphics::GraphicContext& context, InputOutputs& inputOutputs);

            ~Water() = default;
        };
    }
}
