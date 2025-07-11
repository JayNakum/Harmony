#pragma once

#include "common.h"
#include "Log.h"

#include "CommandContext.h"
#include "ShaderInput.h"

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
                unsigned int width, height;
                graphics::dx12::Texture& inFinalOutput;
                graphics::dx12::Texture& inSwapchainBackbuffer;
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
