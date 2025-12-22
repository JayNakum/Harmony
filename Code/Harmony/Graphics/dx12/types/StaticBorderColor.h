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
            // Maps graphics::StaticBorderColor with D3D12_STATIC_BORDER_COLOR
            inline static D3D12_STATIC_BORDER_COLOR toD3DStaticBorderColor(const StaticBorderColor e)
            {
                switch (e)
                {
                default:
                    return static_cast<D3D12_STATIC_BORDER_COLOR>(-1);
                case StaticBorderColor::TransparentBlack:
                    return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
                case StaticBorderColor::OpaqueBlack:
                    return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
                case StaticBorderColor::OpaqueWhite:
                    return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
                case StaticBorderColor::OpaqueBlackUint:
                    return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK_UINT;
                case StaticBorderColor::OpaqueWhiteUint:
                    return D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE_UINT;
                }
            }
        }
    }
}
