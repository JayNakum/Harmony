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
            // maps graphics::TextureAddressMode and D3D12_TEXTURE_ADDRESS_MODE
            static const std::unordered_map<TextureAddressMode, D3D12_TEXTURE_ADDRESS_MODE> textureAddressMode = {
                { TextureAddressMode::Wrap, D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP },
                { TextureAddressMode::Mirror, D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR },
                { TextureAddressMode::Clamp, D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP },
                { TextureAddressMode::Border, D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER },
                { TextureAddressMode::MirrorOnce, D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE },
            };
        }
    }
}
