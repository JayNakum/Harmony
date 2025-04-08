#pragma once

#include "common.h"
#include "directx.h"

#include "HeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_HEAP_TYPE toD3DHeapType(const HeapType type)
        {
            switch (type)
            {
            case HeapType::Default:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
            case HeapType::Upload:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
            }
        }
    }
}
