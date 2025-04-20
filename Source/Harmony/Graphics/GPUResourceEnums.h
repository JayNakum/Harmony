#pragma once

#include "common.h"

namespace graphics
{
    enum class GPUResourceFlags
    {
        None = 0,
        AllowRenderTarget = 1 << 0,
        AllowDepthStencil = 1 << 1,
        AllowUnorderedAccess = 1 << 2
        // TODO: Add support for the following flags
        // DenyShaderResource,
        // AllowCrossAdapter,
        // AllowSimultaneousAccess,
        // VideoDecodeReferenceOnly,
        // VideoEncodeReferenceOnly,
        // RayTracingAccelerationStructure
    };

    inline static const GPUResourceFlags operator|(GPUResourceFlags a, GPUResourceFlags b)
    {
        return static_cast<GPUResourceFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline static const GPUResourceFlags& operator|=(GPUResourceFlags& a, GPUResourceFlags b)
    {
        a = a | b;
        return a;
    }
    inline static const GPUResourceFlags operator&(GPUResourceFlags a, GPUResourceFlags b)
    {
        return static_cast<GPUResourceFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline static const bool hasFlag(GPUResourceFlags flags, GPUResourceFlags check)
    {
        return (flags & check) != GPUResourceFlags::None;
    }

    inline static const std::string toString(const GPUResourceFlags e)
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

    enum class GPUResourceStates
    {
        Invalid,
        Common,
        VertexAndConstantBuffer,
        IndexBuffer,
        RenderTarget,
        CopySource,
        CopyDestination,
        GenericRead,
        Present
        // TODO: Add more resource states
    };

    inline static const GPUResourceStates operator|(GPUResourceStates a, GPUResourceStates b)
    {
        return static_cast<GPUResourceStates>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline static const GPUResourceStates& operator|=(GPUResourceStates& a, GPUResourceStates b)
    {
        a = a | b;
        return a;
    }
    inline static const GPUResourceStates operator&(GPUResourceStates a, GPUResourceStates b)
    {
        return static_cast<GPUResourceStates>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline static const bool hasFlag(GPUResourceStates flags, GPUResourceStates check)
    {
        return (flags & check) != GPUResourceStates::Invalid;
    }

    inline static const std::string toString(const GPUResourceStates e)
    {
        switch (e)
        {
        default:
            return "";
        case GPUResourceStates::Invalid:
            return "Invalid";
        case GPUResourceStates::Common:
            return "Common";
        case GPUResourceStates::CopySource:
            return "CopySource";
        case GPUResourceStates::CopyDestination:
            return "CopyDestination";
        case GPUResourceStates::GenericRead:
            return "GenericRead";
        case GPUResourceStates::Present:
            return "Present";
        }
    }

    enum class GPUResourceDimension
    {
        Unknown,
        Buffer,
        Texture1D,
        Texture2D,
        Texture3D,

        Texture1DArray,
        Texture2DArray
    };

    inline static const std::string toString(const GPUResourceDimension e)
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
        case GPUResourceDimension::Texture1DArray:
            return "Texture1DArray";
        case GPUResourceDimension::Texture2DArray:
            return "Texture2DArray";
        }
    }
}
