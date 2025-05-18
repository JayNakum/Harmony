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
                graphics::dx12::DescriptorHandle inSRV;

                graphics::dx12::RenderTarget inRenderTarget;
                graphics::dx12::DescriptorHandle inRTV;
            };
            void render(graphics::GraphicContext& context, InputOutputs& inputOutputs);

            ~Present() = default;
        };
    }
}
