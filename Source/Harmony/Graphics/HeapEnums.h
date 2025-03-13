#pragma once

#include "common.h"

namespace graphics
{
    enum class HeapType
    {
        Default, // Local
        Upload, // Host visible
        // TODO: Add support for D3D12_HEAP_TYPE_READBACK, D3D12_HEAP_TYPE_CUSTOM, D3D12_HEAP_TYPE_GPU_UPLOAD
    };

    const std::string toString(const HeapType e)
    {
        switch (e)
        {
        default:
            return "";
        case HeapType::Default:
            return "Default";
        case HeapType::Upload:
            return "Upload";
        }
    }
}
