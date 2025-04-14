#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::RootParameterType with D3D12_ROOT_PARAMETER_TYPE
        inline static const D3D12_ROOT_PARAMETER_TYPE toD3DRootParameterType(const RootParameterType e)
        {
            switch (e)
            {
            case RootParameterType::DescriptorTable:
                return D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            case RootParameterType::Constants32:
                return D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            case RootParameterType::CBV:
                return D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_CBV;
            case RootParameterType::SRV:
                return D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_SRV;
            case RootParameterType::UAV:
                return D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_UAV;
            }
        }
    }
}
