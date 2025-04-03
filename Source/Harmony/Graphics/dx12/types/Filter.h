#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace sampler
    {
        namespace dx12
        {
            // maps graphics::filter and D3D12_FILTER
            static const std::unordered_map<Filter, D3D12_FILTER> filter = {
                { Filter::MinMagMipPoint, D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT },
                { Filter::MinMagMipLinear, D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR },
                { Filter::Anisotropic, D3D12_FILTER::D3D12_FILTER_ANISOTROPIC }
            };
        }
    }
}
