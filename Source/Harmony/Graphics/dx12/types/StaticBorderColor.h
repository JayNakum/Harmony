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
            inline static const D3D12_STATIC_BORDER_COLOR toD3DStaticBorderColor(const StaticBorderColor color)
            {
                switch (color)
                {
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
