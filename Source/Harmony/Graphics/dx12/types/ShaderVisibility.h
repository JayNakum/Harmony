#pragma once

#include "common.h"
#include "directx.h"

#include "RootSignatureEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::ShaderVisibility and D3D12_SHADER_VISIBILITY
        static const std::unordered_map<graphics::ShaderVisibility, D3D12_SHADER_VISIBILITY> shaderVisibility = {
            { graphics::ShaderVisibility::All, D3D12_SHADER_VISIBILITY_ALL },
            { graphics::ShaderVisibility::Vertex, D3D12_SHADER_VISIBILITY_VERTEX },
            { graphics::ShaderVisibility::Hull, D3D12_SHADER_VISIBILITY_HULL },
            { graphics::ShaderVisibility::Domain, D3D12_SHADER_VISIBILITY_DOMAIN },
            { graphics::ShaderVisibility::Geometry, D3D12_SHADER_VISIBILITY_GEOMETRY },
            { graphics::ShaderVisibility::Pixel, D3D12_SHADER_VISIBILITY_PIXEL },
            { graphics::ShaderVisibility::Amplification, D3D12_SHADER_VISIBILITY_AMPLIFICATION },
            { graphics::ShaderVisibility::Mesh, D3D12_SHADER_VISIBILITY_MESH }
        };
    }
}
