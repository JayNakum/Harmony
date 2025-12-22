#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::GPUResourceFlags with D3D12_RESOURCE_FLAGS
        inline static D3D12_RESOURCE_FLAGS toD3DResourceFlags(const GPUResourceFlags e)
        {
            D3D12_RESOURCE_FLAGS d3dFlags = D3D12_RESOURCE_FLAG_NONE;

            if (hasFlag(e, GPUResourceFlags::AllowRenderTarget))
            {
                d3dFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            }
            if (hasFlag(e, GPUResourceFlags::AllowDepthStencil))
            {
                d3dFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            }
            if (hasFlag(e, GPUResourceFlags::AllowUnorderedAccess))
            {
                d3dFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            }

            return d3dFlags;
        }
    }
}
