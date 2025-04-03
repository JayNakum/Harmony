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
            { graphics::DescriptorHeapFlags::None, D3D12_DESCRIPTOR_HEAP_FLAG_NONE },
            { graphics::DescriptorHeapFlags::GPUVisible, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE }
        };

        inline static const D3D12_DESCRIPTOR_HEAP_FLAGS toD3DFlags(const graphics::DescriptorHeapFlags flags)
        {
            D3D12_DESCRIPTOR_HEAP_FLAGS d3dFlags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            for (const auto& flag : descriptorHeapFlag)
            {
                if (hasFlag(flags, flag.first))
                {
                    d3dFlags |= flag.second;
                }
            }

            return d3dFlags;
        }
    }
}
