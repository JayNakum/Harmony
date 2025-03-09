#pragma once

namespace graphics
{
    enum GPUResourceFlags
    {
        None,
        AllowRenderTarget,
        AllowDepthStencil,
        AllowUnorderedAccess
        // TODO: Add support for the following flags
        // DenyShaderResource,
        // AllowCrossAdapter,
        // AllowSimultaneousAccess,
        // VideoDecodeReferenceOnly,
        // VideoEncodeReferenceOnly,
        // RayTracingAccelerationStructure
    };

    enum GPUResourceState
    {
        Invalid,
        Common,
        CopyDest,
        GenericRead
        // TODO: Add more resource states
    };
}
