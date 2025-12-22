#pragma once

#include "common.h"
#include "directx.h"

#include "DescriptorHeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::DescriptorHeapType with D3D12_DESCRIPTOR_HEAP_TYPE
        inline static D3D12_DESCRIPTOR_HEAP_TYPE toD3DDescriptorHeapType(const DescriptorHeapType e)
        {
            switch (e)
            {
            default:
            case DescriptorHeapType::ShaderViews:
                return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            case DescriptorHeapType::Sampler:
                return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            case DescriptorHeapType::RenderTarget:
                return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            case DescriptorHeapType::DepthStencil:
                return D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            }
        }
    }
}
