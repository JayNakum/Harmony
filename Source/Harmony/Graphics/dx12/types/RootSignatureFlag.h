#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::RootSignatureFlag and D3D12_ROOT_SIGNATURE_FLAGS
        static const std::unordered_map<RootSignatureFlag, D3D12_ROOT_SIGNATURE_FLAGS> rootSignatureFlag = {
            { RootSignatureFlag::None, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE },
            { RootSignatureFlag::AllowInputAssemblerInputLayout, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT },
            { RootSignatureFlag::DenyVertexShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS },
            { RootSignatureFlag::DenyHullShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS },
            { RootSignatureFlag::DenyDomainShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS },
            { RootSignatureFlag::DenyGeometryShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS },
            { RootSignatureFlag::DenyPixelShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS },
            { RootSignatureFlag::AllowStreamOutput, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT },
            { RootSignatureFlag::LocalRootSignature, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE },
            { RootSignatureFlag::DenyAmplificationShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS },
            { RootSignatureFlag::DenyMeshShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS },
            { RootSignatureFlag::ShaderViewHeapDirectlyIndexed, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED },
            { RootSignatureFlag::SamplerHeapDirectlyIndexed, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED }
        };

        inline static const D3D12_ROOT_SIGNATURE_FLAGS toD3DFlags(const RootSignatureFlag flags)
        {
            D3D12_ROOT_SIGNATURE_FLAGS d3dFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

            for (const auto& flag : rootSignatureFlag)
            {
                if (hasFlag(flags, flag.first))
                {
                    d3dFlags |= flag.second;
                }
            }

            return d3dFlags;
        }
    }
}
