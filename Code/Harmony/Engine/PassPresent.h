#pragma once

#include "CommandContext.h"

namespace engine
{
    namespace renderPass
    {
        class Present
        {
        public:
            Present() = default;

            struct InputOutputs
            {
                unsigned int width;
                unsigned int height;

                graphics::dx12::GPUTexture inPresentTexture;

                graphics::dx12::DescriptorHandle inRTV;
            };
            void render(graphics::GraphicContext& context, InputOutputs& inputOutputs);

            ~Present() = default;
        };
    }
}
