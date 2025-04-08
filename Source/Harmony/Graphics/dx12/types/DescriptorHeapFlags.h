#pragma once

#include "common.h"
#include "directx.h"

#include "DescriptorHeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_DESCRIPTOR_HEAP_FLAGS toD3DDescriptorHeapFlags(const DescriptorHeapFlags flags)
        {
            if (flags == DescriptorHeapFlags::GPUVisible)
            {
                return (D3D12_DESCRIPTOR_HEAP_FLAG_NONE | D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
            }
            return D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        }
    }
}
