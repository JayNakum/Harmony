#pragma once

#include "CommandContext.h"
#include "CommonConstants.h"

namespace engine
{
    namespace renderPass
    {
        class Present
        {
        public:
            struct Inputs
            {
                FrameConstants frame;
                PassConstants pass;
                graphics::dx12::GPUTexture& inFinalOutput;
                graphics::dx12::GPUTexture& inSwapchainBackbuffer;
                graphics::dx12::DescriptorHandle inSwapchainRTV;
            };

        public:
            Present(graphics::GraphicContext& context);

            void render(graphics::GraphicContext& context, Inputs& passInputs);

            ~Present() = default;

        private:
            graphics::dx12::GraphicsPipelineState initialize(graphics::GraphicContext& context);

        private:
            graphics::dx12::GraphicsPipelineState mPresentPSO;

            graphics::dx12::DescriptorHandle mFinalSRV;
        };
    }
}
