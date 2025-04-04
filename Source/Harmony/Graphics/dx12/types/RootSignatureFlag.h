#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::RootSignatureFlags and D3D12_ROOT_SIGNATURE_FLAGS
        static const std::unordered_map<RootSignatureFlags, D3D12_ROOT_SIGNATURE_FLAGS> rootSignatureFlag = {
            { RootSignatureFlags::None, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE },
            { RootSignatureFlags::AllowInputAssemblerInputLayout, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT },
            { RootSignatureFlags::DenyVertexShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS },
            { RootSignatureFlags::DenyHullShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS },
            { RootSignatureFlags::DenyDomainShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS },
            { RootSignatureFlags::DenyGeometryShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS },
            { RootSignatureFlags::DenyPixelShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS },
            { RootSignatureFlags::AllowStreamOutput, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT },
            { RootSignatureFlags::LocalRootSignature, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE },
            { RootSignatureFlags::DenyAmplificationShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS },
            { RootSignatureFlags::DenyMeshShaderRootAccess, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS },
            { RootSignatureFlags::ShaderViewHeapDirectlyIndexed, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED },
            { RootSignatureFlags::SamplerHeapDirectlyIndexed, D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED }
        };

        inline static const D3D12_ROOT_SIGNATURE_FLAGS toD3DFlags(const RootSignatureFlags flags)
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
