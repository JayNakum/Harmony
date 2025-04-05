#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_RESOURCE_FLAGS toD3DResourceFlags(const GPUResourceFlags flags)
        {
            switch (flags)
            {
            case GPUResourceFlags::None:
                return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE;
            case GPUResourceFlags::AllowRenderTarget:
                return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            case GPUResourceFlags::AllowDepthStencil:
                return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            case GPUResourceFlags::AllowUnorderedAccess:
                return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            // TODO: Add support for the following flags when implemented in GPUResourceFlags
            // case GPUResourceFlags::DenyShaderResource:
            //     return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
            // case GPUResourceFlags::AllowCrossAdapter:
            //     return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
            // case GPUResourceFlags::AllowSimultaneousAccess:
            //     return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
            // case GPUResourceFlags::VideoDecodeReferenceOnly:
            //     return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY;
            // case GPUResourceFlags::VideoEncodeReferenceOnly:
            //     return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_VIDEO_ENCODE_REFERENCE_ONLY;
            // case GPUResourceFlags::RayTracingAccelerationStructure:
            //     return D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;
            }
        }

    }
}
