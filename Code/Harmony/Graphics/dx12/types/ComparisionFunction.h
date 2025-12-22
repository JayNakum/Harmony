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
            // Maps graphics::ComparisionFunction with D3D12_COMPARISION_FUNC
            inline static D3D12_COMPARISON_FUNC toD3DComparisonFunc(const ComparisionFunction e)
            {
                switch (e)
                {
                default:
                case ComparisionFunction::None:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NONE;
                case ComparisionFunction::Never:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
                case ComparisionFunction::Less:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
                case ComparisionFunction::Equal:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
                case ComparisionFunction::LessEqual:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
                case ComparisionFunction::Greater:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
                case ComparisionFunction::NotEqual:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL;
                case ComparisionFunction::GreaterEqual:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
                case ComparisionFunction::Always:
                    return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
                }
            }
        }
    }
}
