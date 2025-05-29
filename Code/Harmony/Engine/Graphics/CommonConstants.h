#pragma once

#include "common.h"
#include "Maths.h"

namespace engine
{
    struct FrameConstants
    {
        float totalTime = 0.0f;
        float deltaTime = 0.0f;

        uint64_t frameIndex = 0;
    };
    static_assert(sizeof(FrameConstants) % 16 == 0, "Must be aligned");

    struct PassConstants
    {
        mat4f view;
        mat4f invView;

        mat4f projection;
        mat4f invProjection;

        vec3f eyePosition;
        const uint32_t _padding0 = 0x1deadb0b;

        vec2f renderTargetSize;
        vec2f invRenderTargetSize;

        float nearZ = 0.0f;
        float farZ = 0.0f;
        const uint64_t _padding1 = 0x1deadb0b1deadb0b;
    };
    static_assert(sizeof(PassConstants) % 16 == 0, "Must be aligned");
}
