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
        // DenyShaderResource,
        // AllowCrossAdapter,
        // AllowSimultaneousAccess,
        // VideoDecodeReferenceOnly,
        // VideoEncodeReferenceOnly,
        // RayTracingAccelerationStructure
    };

    inline static GPUResourceFlags operator|(GPUResourceFlags a, GPUResourceFlags b)
    {
        return static_cast<GPUResourceFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline static GPUResourceFlags& operator|=(GPUResourceFlags& a, GPUResourceFlags b)
    {
        a = a | b;
        return a;
    }
    inline static GPUResourceFlags operator&(GPUResourceFlags a, GPUResourceFlags b)
    {
        return static_cast<GPUResourceFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline static bool hasFlag(GPUResourceFlags flags, GPUResourceFlags check)
    {
        return (flags & check) != GPUResourceFlags::None;
    }

    inline static std::string toString(const GPUResourceFlags e)
    {
        std::string result;

        if (hasFlag(e, GPUResourceFlags::None))
        {
            result += "None";
        }
        if (hasFlag(e, GPUResourceFlags::AllowRenderTarget))
        {
            result += "AllowRenderTarget";
        }
        if (hasFlag(e, GPUResourceFlags::AllowDepthStencil))
        {
            result += "AllowDepthStencil";
        }
        if (hasFlag(e, GPUResourceFlags::AllowUnorderedAccess))
        {
            result += "AllowUnorderedAccess";
        }
        if (result.empty())
        {
            result = "";
        }

        return result;
    }

    enum class GPUResourceStates
    {
        Common = 0,
        VertexAndConstantBuffer = 1 << 0,
        IndexBuffer = 1 << 1,
        RenderTarget = 1 << 2,
        UnorderedAccess = 1 << 3,
        DepthWrite = 1 << 4,
        DepthRead = 1 << 5,
        NonPixelShaderResource = 1 << 6,
        PixelShaderResource = 1 << 7,
        StreamOut = 1 << 8,
        IndirectArgument = 1 << 9,
        CopyDestination = 1 << 10,
        CopySource = 1 << 11,
        ResolveDestination = 1 << 12,
        ResolveSource = 1 << 13,
        ShadingRateResource = 1 << 14,
        GenericRead = 1 << 15,// (((((VertexAndConstantBuffer | IndexBuffer) | NonPixelShaderResource) | PixelShaderResource) | IndirectArgument) | CopySource),
        AllShaderResource = 1 << 16, // (NonPixelShaderResource | PixelShaderResource),
        Present = 1 << 17,
        Predication = 1 << 18,
    };

    inline static GPUResourceStates operator|(GPUResourceStates a, GPUResourceStates b)
    {
        return static_cast<GPUResourceStates>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline static GPUResourceStates& operator|=(GPUResourceStates& a, GPUResourceStates b)
    {
        a = a | b;
        return a;
    }
    inline static GPUResourceStates operator&(GPUResourceStates a, GPUResourceStates b)
    {
        return static_cast<GPUResourceStates>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline static bool hasFlag(GPUResourceStates flags, GPUResourceStates check)
    {
        return (flags & check) != GPUResourceStates::Common;
    }

    inline static std::string toString(const GPUResourceStates e)
    {
        std::string result;

        if (hasFlag(e, GPUResourceStates::Common))
        {
            result += "Common";
        }
        if (hasFlag(e, GPUResourceStates::VertexAndConstantBuffer))
        {
            result += "VertexAndConstantBuffer";
        }
        if (hasFlag(e, GPUResourceStates::IndexBuffer))
        {
            result += "IndexBuffer";
        }
        if (hasFlag(e, GPUResourceStates::RenderTarget))
        {
            result += "RenderTarget";
        }
        if (hasFlag(e, GPUResourceStates::UnorderedAccess))
        {
            result += "UnorderedAccess";
        }
        if (hasFlag(e, GPUResourceStates::DepthWrite))
        {
            result += "DepthWrite";
        }
        if (hasFlag(e, GPUResourceStates::DepthRead))
        {
            result += "DepthRead";
        }
        if (hasFlag(e, GPUResourceStates::NonPixelShaderResource))
        {
            result += "NonPixelShaderResource";
        }
        if (hasFlag(e, GPUResourceStates::PixelShaderResource))
        {
            result += "PixelShaderResource";
        }
        if (hasFlag(e, GPUResourceStates::StreamOut))
        {
            result += "StreamOut";
        }
        if (hasFlag(e, GPUResourceStates::IndirectArgument))
        {
            result += "IndirectArgument";
        }
        if (hasFlag(e, GPUResourceStates::CopyDestination))
        {
            result += "CopyDestination";
        }
        if (hasFlag(e, GPUResourceStates::CopySource))
        {
            result += "CopySource";
        }
        if (hasFlag(e, GPUResourceStates::ResolveDestination))
        {
            result += "ResolveDestination";
        }
        if (hasFlag(e, GPUResourceStates::ResolveSource))
        {
            result += "ResolveSource";
        }
        if (hasFlag(e, GPUResourceStates::ShadingRateResource))
        {
            result += "ShadingRateResource";
        }
        if (hasFlag(e, GPUResourceStates::GenericRead))
        {
            result += "GenericRead";
        }
        if (hasFlag(e, GPUResourceStates::AllShaderResource))
        {
            result += "AllShaderResource";
        }
        if (hasFlag(e, GPUResourceStates::Present))
        {
            result += "Present";
        }
        if (hasFlag(e, GPUResourceStates::Predication))
        {
            result += "Predication";
        }
        if (result.empty())
        {
            result = "";
        }

        return result;
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

    inline static std::string toString(const GPUResourceDimension e)
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
