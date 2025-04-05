#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_SHADER_VISIBILITY toD3DShaderVisibility(const ShaderVisibility visibility)
        {
            switch (visibility)
            {
            case ShaderVisibility::All:
                return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
            case ShaderVisibility::Vertex:
                return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
            case ShaderVisibility::Hull:
                return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_HULL;
            case ShaderVisibility::Domain:
                return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_DOMAIN;
            case ShaderVisibility::Geometry:
                return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_GEOMETRY;
            case ShaderVisibility::Pixel:
                return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
            case ShaderVisibility::Amplification:
                return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_AMPLIFICATION;
            case ShaderVisibility::Mesh:
                return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH;
            }
        }
    }
}
