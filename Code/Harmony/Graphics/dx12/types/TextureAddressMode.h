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
            // Maps graphics::TextureAddressMode with D3D12_TEXTURE_ADDRESS_MODE
            inline static D3D12_TEXTURE_ADDRESS_MODE toD3DTextureAddressMode(const TextureAddressMode e)
            {
                switch (e)
                {
                default:
                    return static_cast<D3D12_TEXTURE_ADDRESS_MODE>(-1);
                case TextureAddressMode::Wrap:
                    return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                case TextureAddressMode::Mirror:
                    return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
                case TextureAddressMode::Clamp:
                    return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
                case TextureAddressMode::Border:
                    return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
                case TextureAddressMode::MirrorOnce:
                    return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
                }
            }
        }
    }
}
