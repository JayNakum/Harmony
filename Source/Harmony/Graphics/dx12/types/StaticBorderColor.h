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
            // maps graphics::StaticBorderColor and D3D12_STATIC_BORDER_COLOR
            static const std::unordered_map<StaticBorderColor, D3D12_STATIC_BORDER_COLOR> staticBorderColor = {
                { StaticBorderColor::TransparentBlack, D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK },
                { StaticBorderColor::OpaqueBlack, D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK },
                { StaticBorderColor::OpaqueWhite, D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE },
                { StaticBorderColor::OpaqueBlackUint, D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK_UINT },
                { StaticBorderColor::OpaqueWhiteUint, D3D12_STATIC_BORDER_COLOR::D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE_UINT }
            };
        }
    }
}
