#pragma once

#include "common.h"
#include "Maths.h"

namespace engine
{
    struct FrameConstants
    {
        float totalTime = 0.0f;
        float deltaTime = 0.0f;
        const uint32_t _framePadding0 = 0x1deadb0b;
        const uint32_t _framePadding1 = 0x1deadb0b;

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
        const std::array<uint32_t, 20> _passPaddingFinal = { 0x1deadb0b };
    };
    static_assert(sizeof(PassConstants) % 256 == 0, "Must be aligned");
}
