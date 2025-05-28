#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::ShaderVisibility with D3D12_SHADER_VISIBILITY
        inline static const D3D12_SHADER_VISIBILITY toD3DShaderVisibility(const ShaderVisibility e)
        {
            unsigned int count = 0;
            D3D12_SHADER_VISIBILITY result = D3D12_SHADER_VISIBILITY_ALL;
            if (hasFlag(e, ShaderVisibility::Vertex))
            {
                result = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
                ++count;
            }
            if (hasFlag(e, ShaderVisibility::Hull))
            {
                result = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_HULL;
                ++count;
            }
            if (hasFlag(e, ShaderVisibility::Domain))
            {
                result = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_DOMAIN;
                ++count;
            }
            if (hasFlag(e, ShaderVisibility::Geometry))
            {
                result = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_GEOMETRY;
                ++count;
            }
            if (hasFlag(e, ShaderVisibility::Pixel))
            {
                result = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
                ++count;
            }
            if (hasFlag(e, ShaderVisibility::Amplification))
            {
                result = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_AMPLIFICATION;
                ++count;
            }
            if (hasFlag(e, ShaderVisibility::Mesh))
            {
                result = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_MESH;
                ++count;
            }

            if (count != 1 || hasFlag(e, ShaderVisibility::All))
            {
                result = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
            }

            return result;
        }
    }
}
