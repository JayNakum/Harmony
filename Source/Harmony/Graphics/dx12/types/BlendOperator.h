#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_BLEND_OP toD3DBlendOperator(const BlendOperator blendOperator)
        {
            switch (blendOperator)
            {
            case BlendOperator::Add:
                return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
            case BlendOperator::Subtract:
                return D3D12_BLEND_OP::D3D12_BLEND_OP_SUBTRACT;
            case BlendOperator::ReverseSubtract:
                return D3D12_BLEND_OP::D3D12_BLEND_OP_REV_SUBTRACT;
            case BlendOperator::Min:
                return D3D12_BLEND_OP::D3D12_BLEND_OP_MIN;
            case BlendOperator::Max:
                return D3D12_BLEND_OP::D3D12_BLEND_OP_MAX;
            }
        }
    }
}
