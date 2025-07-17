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
    class RenderPass
    {
    protected:
        virtual void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager) = 0;
        virtual void freeGPUUploadData(ResourceManager& resourceManager) = 0;

        virtual void resize(const unsigned int width, const unsigned int height, graphics::dx12::Device& device, ResourceManager& resourceManager) = 0;
        virtual void update() = 0;

        virtual void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) = 0;
        virtual void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) = 0;
    };
}
