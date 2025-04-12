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
            D3D12_RESOURCE_FLAGS d3dFlags = D3D12_RESOURCE_FLAG_NONE;

            if (hasFlag(flags, GPUResourceFlags::AllowRenderTarget))
            {
                d3dFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            }
            if (hasFlag(flags, GPUResourceFlags::AllowDepthStencil))
            {
                d3dFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            }
            if (hasFlag(flags, GPUResourceFlags::AllowUnorderedAccess))
            {
                d3dFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            }

            return d3dFlags;
        }
    }
}
