#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

namespace graphics
{
    namespace dx12
    {
        static const std::unordered_map<graphics::GPUResourceDimension, D3D12_RESOURCE_DIMENSION> gpuResourceDimension = {
            { GPUResourceDimension::Unknown, D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN },
            { GPUResourceDimension::Buffer, D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER },
            { GPUResourceDimension::Texture1D, D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D },
            { GPUResourceDimension::Texture2D, D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D },
            { GPUResourceDimension::Texture3D, D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D }
        };
    }
}
