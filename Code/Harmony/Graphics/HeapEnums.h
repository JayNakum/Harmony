#pragma once

#include "common.h"

namespace graphics
{
    enum class HeapType
    {
        Default, // Local
        Upload, // Host visible
        Readback,
        GPUUpload,
        Custom
    };

    inline static const std::string toString(const HeapType e)
    {
        switch (e)
        {
        default:
            return "";
        case HeapType::Default:
            return "Default";
        case HeapType::Upload:
            return "Upload";
        case HeapType::Readback:
            return "Readback";
        case HeapType::GPUUpload:
            return "GPUUpload";
        case HeapType::Custom:
            return "Custom";
        }
    }
}
