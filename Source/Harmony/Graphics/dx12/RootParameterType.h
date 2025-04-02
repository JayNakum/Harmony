#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::RootParameterType and D3D12_ROOT_PARAMETER_TYPE
        static const std::unordered_map<graphics::RootParameterType, D3D12_ROOT_PARAMETER_TYPE> rootParameterType = {
            { graphics::RootParameterType::DescriptorTable, D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE },
            { graphics::RootParameterType::Constants32, D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS },
            { graphics::RootParameterType::CBV, D3D12_ROOT_PARAMETER_TYPE_CBV },
            { graphics::RootParameterType::SRV, D3D12_ROOT_PARAMETER_TYPE_SRV },
            { graphics::RootParameterType::UAV, D3D12_ROOT_PARAMETER_TYPE_UAV }
        };
    }
}
