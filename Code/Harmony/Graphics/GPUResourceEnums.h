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
        return (flags & check) != GPUResourceStates::Common;
    }

    inline static const std::string toString(const GPUResourceStates e)
    {
        switch (e)
        {
        default:
            return "";
        case GPUResourceStates::Common:
            return "Common";
        case GPUResourceStates::VertexAndConstantBuffer:
            return "VertexAndConstantBuffer";
        case GPUResourceStates::IndexBuffer:
            return "IndexBuffer";
        case GPUResourceStates::RenderTarget:
            return "RenderTarget";
        case GPUResourceStates::UnorderedAccess:
            return "UnorderedAccess";
        case GPUResourceStates::DepthWrite:
            return "DepthWrite";
        case GPUResourceStates::DepthRead:
            return "DepthRead";
        case GPUResourceStates::NonPixelShaderResource:
            return "NonPixelShaderResource";
        case GPUResourceStates::PixelShaderResource:
            return "PixelShaderResource";
        case GPUResourceStates::StreamOut:
            return "StreamOut";
        case GPUResourceStates::IndirectArgument:
            return "IndirectArgument";
        case GPUResourceStates::CopyDestination:
            return "CopyDestination";
        case GPUResourceStates::CopySource:
            return "CopySource";
        case GPUResourceStates::ResolveDestination:
            return "ResolveDestination";
        case GPUResourceStates::ResolveSource:
            return "ResolveSource";
        case GPUResourceStates::ShadingRateResource:
            return "ShadingRateResource";
        case GPUResourceStates::GenericRead:
            return "GenericRead";
        case GPUResourceStates::AllShaderResource:
            return "AllShaderResource";
        case GPUResourceStates::Present:
            return "Present";
        case GPUResourceStates::Predication:
            return "Predication";
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
