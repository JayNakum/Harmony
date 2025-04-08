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
            inline static const D3D12_TEXTURE_ADDRESS_MODE toD3DTextureAddressMode(const TextureAddressMode mode)
            {
                switch (mode)
                {
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
