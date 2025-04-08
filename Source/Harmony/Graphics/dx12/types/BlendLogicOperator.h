#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_LOGIC_OP toD3DBlendLogicOperator(const BlendLogicOperator blendLogicOperator)
        {
            switch (blendLogicOperator)
            {
            case BlendLogicOperator::Clear:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_CLEAR;
            case BlendLogicOperator::Set:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_SET;
            case BlendLogicOperator::Copy:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_COPY;
            case BlendLogicOperator::CopyInverted:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_COPY_INVERTED;
            case BlendLogicOperator::NOOP:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
            case BlendLogicOperator::Invert:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_INVERT;
            case BlendLogicOperator::AND:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_AND;
            case BlendLogicOperator::NAND:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_NAND;
            case BlendLogicOperator::OR:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_OR;
            case BlendLogicOperator::NOR:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOR;
            case BlendLogicOperator::XOR:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_XOR;
            case BlendLogicOperator::EQUIV:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_EQUIV;
            case BlendLogicOperator::ANDReverse:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_AND_REVERSE;
            case BlendLogicOperator::ANDInverted:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_AND_INVERTED;
            case BlendLogicOperator::ORReverse:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_OR_REVERSE;
            case BlendLogicOperator::ORInverted:
                return D3D12_LOGIC_OP::D3D12_LOGIC_OP_OR_INVERTED;
            }
        }
    }
}
