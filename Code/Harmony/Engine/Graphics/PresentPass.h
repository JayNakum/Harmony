#pragma once

#include "CommandContext.h"

namespace engine
{
    namespace renderPass
    {
        class Present
        {
        public:
            Present(graphics::GraphicContext& context);

            struct InputOutputs
            {
                graphics::dx12::DescriptorHandle inRTV;

                graphics::dx12::DescriptorHandle inPresentTextureSRV;
            };
            void render(graphics::GraphicContext& context, InputOutputs& inputOutputs);

            ~Present() = default;

        private:
            graphics::dx12::GraphicsPipelineState initialize(graphics::GraphicContext& context);

        private:
            graphics::dx12::GraphicsPipelineState mPresentPSO;
        };
    }
}
