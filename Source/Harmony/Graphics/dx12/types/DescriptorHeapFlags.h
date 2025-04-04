#pragma once

#include "common.h"
#include "directx.h"

#include "DescriptorHeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::DescriptorHeapFlags and D3D12_DESCRIPTOR_HEAP_FLAGS
        static const std::unordered_map<graphics::DescriptorHeapFlags, D3D12_DESCRIPTOR_HEAP_FLAGS> descriptorHeapFlag = {
            { graphics::DescriptorHeapFlags::CPUVisible, D3D12_DESCRIPTOR_HEAP_FLAG_NONE },
            { graphics::DescriptorHeapFlags::GPUVisible, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE }
        };

        inline static const D3D12_DESCRIPTOR_HEAP_FLAGS toD3DFlags(const graphics::DescriptorHeapFlags flags)
        {
            return (flags == DescriptorHeapFlags::GPUVisible) ? (D3D12_DESCRIPTOR_HEAP_FLAG_NONE | D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        }
    }
}
