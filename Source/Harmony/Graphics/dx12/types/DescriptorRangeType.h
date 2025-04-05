#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_DESCRIPTOR_RANGE_TYPE toD3DDescriptorRangeType(const DescriptorRangeType type)
        {
            switch (type)
            {
            case DescriptorRangeType::CBV:
                return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            case DescriptorRangeType::SRV:
                return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            case DescriptorRangeType::UAV:
                return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            case DescriptorRangeType::Sampler:
                return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            }
        }
    }
}
