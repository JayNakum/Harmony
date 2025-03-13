#pragma once

#include "common.h"

namespace graphics
{
    enum class GPUResourceFlags
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

    const std::string toString(const GPUResourceFlags e)
    {
        switch (e)
        {
        default:
            return "";
        case GPUResourceFlags::None:
            return "None";
        case GPUResourceFlags::AllowRenderTarget:
            return "AllowRenderTarget";
        case GPUResourceFlags::AllowDepthStencil:
            return "AllowDepthStencil";
        case GPUResourceFlags::AllowUnorderedAccess:
            return "AllowUnorderedAccess";
        }
    }

    enum class GPUResourceState
    {
        Invalid,
        Common,
        CopyDest,
        GenericRead
        // TODO: Add more resource states
    };

    const std::string toString(const GPUResourceState e)
    {
        switch (e)
        {
        default:
            return "";
        case GPUResourceState::Invalid:
            return "Invalid";
        case GPUResourceState::Common:
            return "Common";
        case GPUResourceState::CopyDest:
            return "CopyDest";
        case GPUResourceState::GenericRead:
            return "GenericRead";
        }
    }

    enum class GPUResourceDimension
    {
        Unknown,
        Buffer,
        Texture1D,
        Texture2D,
        Texture3D
    };

    const std::string toString(const GPUResourceDimension e)
    {
        switch (e)
        {
        default:
            return "";
        case GPUResourceDimension::Unknown:
            return "Unknown";
        case GPUResourceDimension::Buffer:
            return "Buffer";
        case GPUResourceDimension::Texture1D:
            return "Texture1D";
        case GPUResourceDimension::Texture2D:
            return "Texture2D";
        case GPUResourceDimension::Texture3D:
            return "Texture3D";
        }
    }
}
