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
            // maps graphics::ComparisionFunction and D3D12_COMPARISON_FUNC
            static const std::unordered_map<ComparisionFunction, D3D12_COMPARISON_FUNC> comparisionFunction = {
                { ComparisionFunction::None, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NONE },
                { ComparisionFunction::Never, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER },
                { ComparisionFunction::Less, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS },
                { ComparisionFunction::Equal, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL },
                { ComparisionFunction::LessEqual, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL },
                { ComparisionFunction::Greater, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER },
                { ComparisionFunction::NotEqual, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL },
                { ComparisionFunction::GreaterEqual, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL },
                { ComparisionFunction::Always, D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS }
            };
        }
    }
}
