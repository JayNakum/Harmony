#pragma once

#include "common.h"
#include "Log.h"
#include "Maths.h"

#include "ResourceManager.h"
#include "CommandContext.h"

#include "ShaderInput.h"
#include "RenderItem.h"

namespace engine
{
    /*
    * Common root constants that every pass is suggested to have.
    */

    struct FrameConstants
    {
        float totalTime = 0.0f;
        float deltaTime = 0.0f;
        const uint32_t _framePaddingFinal[2] = { 0x1deadb0b, 0x1deadb0b };
    };
    static_assert(sizeof(FrameConstants) % 16 == 0, "Must be aligned");

    struct PassConstants
    {
        mat4f view;
        mat4f invView;

        vec3f eyePosition;
        const uint32_t _passPadding0 = 0x1deadb0b;

        mat4f projection;
        mat4f invProjection;

        float nearZ = 0.0f;
        float farZ = 0.0f;
        const uint32_t _passPadding1 = 0x1deadb0b;
        const uint32_t _passPadding2 = 0x1deadb0b;

        mat4f viewProjection;
        mat4f invViewProjection;

        vec2f renderTargetSize;
        vec2f invRenderTargetSize;
        const uint32_t _passPaddingFinal[20] = {
            0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b,
            0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b,
            0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b,
            0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b, 0x1deadb0b
        };
    };
    static_assert(sizeof(PassConstants) % 256 == 0, "Must be aligned");
}

namespace engine
{
    /*
    * A pass on graphics queue.
    */
    class RenderPass
    {
    public:
        virtual void copyDataToGPU(graphics::GraphicContext& context, ResourceManager& resourceManager) = 0;
        virtual void freeGPUUploadData(ResourceManager& resourceManager) = 0;

        virtual void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) = 0;
        virtual void update(const double timeStep) = 0;

        virtual void getInputOutputs(graphics::GraphicContext& context, ResourceManager& resourceManager) = 0;
        virtual void render(graphics::GraphicContext& context, const FrameConstants& frameConstants) = 0;

        virtual ~RenderPass() = default;
    };
}

namespace engine
{
    /*
    * A pass on compute queue.
    */
    class DispatchPass
    {
    public:
        virtual void copyDataToGPU(graphics::ComputeContext& context, ResourceManager& resourceManager) = 0;
        virtual void freeGPUUploadData(ResourceManager& resourceManager) = 0;

        virtual void resize(const unsigned int width, const unsigned int height, graphics::Device& device, ResourceManager& resourceManager) = 0;
        virtual void update(const double timeStep) = 0;

        virtual void getInputOutputs(graphics::ComputeContext& context, ResourceManager& resourceManager) = 0;
        virtual void dispatch(graphics::ComputeContext& context, const FrameConstants& frameConstants) = 0;

        virtual ~DispatchPass() = default;
    };
}
