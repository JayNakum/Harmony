#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::DescriptorRangeType and D3D12_DESCRIPTOR_RANGE_TYPE
        static const std::unordered_map<graphics::DescriptorRangeType, D3D12_DESCRIPTOR_RANGE_TYPE> descriptorRangeType = {
            { graphics::DescriptorRangeType::CBV, D3D12_DESCRIPTOR_RANGE_TYPE_CBV },
            { graphics::DescriptorRangeType::SRV, D3D12_DESCRIPTOR_RANGE_TYPE_SRV },
            { graphics::DescriptorRangeType::UAV, D3D12_DESCRIPTOR_RANGE_TYPE_UAV },
            { graphics::DescriptorRangeType::Sampler, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER }
        };
    }
}
