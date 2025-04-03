#pragma once

#include "common.h"
#include "directx.h"

#include "DescriptorHeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::DescriptorHeapFlag and D3D12_DESCRIPTOR_HEAP_FLAGS
        static const std::unordered_map<graphics::DescriptorHeapFlag, D3D12_DESCRIPTOR_HEAP_FLAGS> descriptorHeapFlag = {
            { graphics::DescriptorHeapFlag::CPUVisible, D3D12_DESCRIPTOR_HEAP_FLAG_NONE },
            { graphics::DescriptorHeapFlag::GPUVisible, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE }
        };
    }
}
