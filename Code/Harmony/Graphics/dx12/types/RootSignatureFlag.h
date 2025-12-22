#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::RootSignatureFlags with D3D12_ROOT_SIGNATURE_FLAGS
        inline static D3D12_ROOT_SIGNATURE_FLAGS toD3DRootSignatureFlags(const RootSignatureFlags e)
        {
            D3D12_ROOT_SIGNATURE_FLAGS d3dFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

            if (hasFlag(e, RootSignatureFlags::AllowInputAssemblerInputLayout))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
            }
            if (hasFlag(e, RootSignatureFlags::DenyVertexShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(e, RootSignatureFlags::DenyHullShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(e, RootSignatureFlags::DenyDomainShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(e, RootSignatureFlags::DenyGeometryShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(e, RootSignatureFlags::DenyPixelShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(e, RootSignatureFlags::AllowStreamOutput))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
            }
            if (hasFlag(e, RootSignatureFlags::LocalRootSignature))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
            }
            if (hasFlag(e, RootSignatureFlags::DenyAmplificationShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(e, RootSignatureFlags::DenyMeshShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(e, RootSignatureFlags::ShaderViewHeapDirectlyIndexed))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
            }
            if (hasFlag(e, RootSignatureFlags::SamplerHeapDirectlyIndexed))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
            }

            return d3dFlags;
        }
    }
}
