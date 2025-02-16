#pragma once

namespace graphics
{
    enum HeapType
    {
        Default,
        Upload,
        // TODO: Add support for D3D12_HEAP_TYPE_READBACK, D3D12_HEAP_TYPE_CUSTOM, D3D12_HEAP_TYPE_GPU_UPLOAD
    };
}
