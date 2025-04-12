#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_ROOT_SIGNATURE_FLAGS toD3DRootSignatureFlags(const RootSignatureFlags flags)
        {
            D3D12_ROOT_SIGNATURE_FLAGS d3dFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

            if (hasFlag(flags, RootSignatureFlags::AllowInputAssemblerInputLayout))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
            }
            if (hasFlag(flags, RootSignatureFlags::DenyVertexShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(flags, RootSignatureFlags::DenyHullShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(flags, RootSignatureFlags::DenyDomainShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(flags, RootSignatureFlags::DenyGeometryShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(flags, RootSignatureFlags::DenyPixelShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(flags, RootSignatureFlags::AllowStreamOutput))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
            }
            if (hasFlag(flags, RootSignatureFlags::LocalRootSignature))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
            }
            if (hasFlag(flags, RootSignatureFlags::DenyAmplificationShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(flags, RootSignatureFlags::DenyMeshShaderRootAccess))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
            }
            if (hasFlag(flags, RootSignatureFlags::ShaderViewHeapDirectlyIndexed))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
            }
            if (hasFlag(flags, RootSignatureFlags::SamplerHeapDirectlyIndexed))
            {
                d3dFlags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
            }

            return d3dFlags;
        }
    }
}
