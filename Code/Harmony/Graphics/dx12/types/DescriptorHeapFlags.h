#pragma once

#include "common.h"
#include "directx.h"

#include "DescriptorHeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::DescriptorHeapFlags with D3D12_DESCRIPTOR_HEAP_FLAGS
        inline static D3D12_DESCRIPTOR_HEAP_FLAGS toD3DDescriptorHeapFlags(const DescriptorHeapFlags e)
        {
            if (e == DescriptorHeapFlags::GPUVisible)
            {
                return (D3D12_DESCRIPTOR_HEAP_FLAG_NONE | D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
            }
            return D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        }
    }
}
