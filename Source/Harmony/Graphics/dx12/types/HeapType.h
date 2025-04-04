#pragma once

#include "common.h"
#include "directx.h"

#include "HeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::HeapType and D3D12_HEAP_TYPE
        static const std::unordered_map<graphics::HeapType, D3D12_HEAP_TYPE> heapType = {
            { graphics::HeapType::Default, D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT },
            { graphics::HeapType::Upload, D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD }
        };
    }
}
