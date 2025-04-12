#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_RESOURCE_DIMENSION toD3DResourceDimension(const GPUResourceDimension dimension)
        {
            switch (dimension)
            {
            default:
            case GPUResourceDimension::Unknown:
                return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN;
            case GPUResourceDimension::Buffer:
                return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
            case GPUResourceDimension::Texture1D:
                return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            case GPUResourceDimension::Texture2D:
                return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            case GPUResourceDimension::Texture3D:
                return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            }
        }

        inline static const D3D12_SRV_DIMENSION toD3DSRVDimension(const GPUResourceDimension dimension)
        {
            switch (dimension)
            {
            default:
            case GPUResourceDimension::Unknown:
                return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_UNKNOWN;
            case GPUResourceDimension::Buffer:
                return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
            case GPUResourceDimension::Texture1D:
                return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1D;
            case GPUResourceDimension::Texture2D:
                return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
            case GPUResourceDimension::Texture3D:
                return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE3D;
            case GPUResourceDimension::Texture1DArray:
                return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            case GPUResourceDimension::Texture2DArray:
                return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            }
        }
    }
}
