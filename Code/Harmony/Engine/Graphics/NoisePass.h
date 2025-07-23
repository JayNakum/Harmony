#pragma once

#include "common.h"
#include "Log.h"

#include "CommonConstants.h"
#include "ResourceManager.h"
#include "CommandContext.h"

#include "ShaderInput.h"
#include "RenderItem.h"

namespace engine
{
    namespace renderPass
    {
        class Noise
        {
        public:
            Noise(graphics::dx12::Device& device, ResourceManager& resourceManager);

            void copyDataToGPU(graphics::ComputeContext& context, ResourceManager& resourceManager);
            void freeGPUUploadData(ResourceManager& resourceManager);

            void resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager);
            void update(const double timeStep);

            void getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager);
            void render(graphics::ComputeContext& context, const FrameConstants& frameConstants);

            ~Noise() = default;
        };
    }
}
