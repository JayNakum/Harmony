#pragma once

#include "common.h"
#include "directx.h"

#include "DescriptorHeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::DescriptorHeapType and D3D12_DESCRIPTOR_HEAP_TYPE
        static const std::unordered_map<graphics::DescriptorHeapType, D3D12_DESCRIPTOR_HEAP_TYPE> descriptorHeapType = {
            { graphics::DescriptorHeapType::View,         D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV },
            { graphics::DescriptorHeapType::Sampler,      D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER },
            { graphics::DescriptorHeapType::RenderTarget, D3D12_DESCRIPTOR_HEAP_TYPE_RTV },
            { graphics::DescriptorHeapType::DepthStencil, D3D12_DESCRIPTOR_HEAP_TYPE_DSV }
        };

    }
}
