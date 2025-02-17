#pragma once

namespace graphics
{
	enum GPUResourcFlags
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
}
