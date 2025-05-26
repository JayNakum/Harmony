#pragma once

#include "common.h"
#include "directx.h"

#include "HeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::HeapType with D3D12_HEAP_TYPE
        inline static const D3D12_HEAP_TYPE toD3DHeapType(const HeapType e)
        {
            switch (e)
            {
            default:
            case HeapType::Default:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
            case HeapType::Upload:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
            case HeapType::Readback:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK;
            case HeapType::GPUUpload:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_GPU_UPLOAD;
            case HeapType::Custom:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
            }
        }
    }
}
