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
            inline static const D3D12_FILTER toD3DFilter(const Filter filter)
            {
                switch (filter)
                {
                case Filter::MinMagMipPoint:
                    return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
                case Filter::MinMagMipLinear:
                    return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
                case Filter::Anisotropic:
                    return D3D12_FILTER::D3D12_FILTER_ANISOTROPIC;
                }
            }
        }
    }
}
