#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::Blend with D3D12_BLEND
        inline static const D3D12_BLEND toD3DBlend(const Blend e)
        {
            switch (e)
            {
            default:
            case Blend::Zero:
                return D3D12_BLEND::D3D12_BLEND_ZERO;
            case Blend::One:
                return D3D12_BLEND::D3D12_BLEND_ONE;
            case Blend::SourceColor:
                return D3D12_BLEND::D3D12_BLEND_SRC_COLOR;
            case Blend::InverseSourceColor:
                return D3D12_BLEND::D3D12_BLEND_INV_SRC_COLOR;
            case Blend::SourceAlpha:
                return D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
            case Blend::InverseSourceAlpha:
                return D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
            case Blend::DestinationAlpha:
                return D3D12_BLEND::D3D12_BLEND_DEST_ALPHA;
            case Blend::InverseDestinationAlpha:
                return D3D12_BLEND::D3D12_BLEND_INV_DEST_ALPHA;
            case Blend::DestinationColor:
                return D3D12_BLEND::D3D12_BLEND_DEST_COLOR;
            case Blend::InverseDestinationColor:
                return D3D12_BLEND::D3D12_BLEND_INV_DEST_COLOR;
            case Blend::SourceAlphaSaturate:
                return D3D12_BLEND::D3D12_BLEND_SRC_ALPHA_SAT;
            case Blend::BlendFactor:
                return D3D12_BLEND::D3D12_BLEND_BLEND_FACTOR;
            case Blend::InverseBlendFactor:
                return D3D12_BLEND::D3D12_BLEND_INV_BLEND_FACTOR;
            case Blend::Source1Color:
                return D3D12_BLEND::D3D12_BLEND_SRC1_COLOR;
            case Blend::InverseSource1Color:
                return D3D12_BLEND::D3D12_BLEND_INV_SRC1_COLOR;
            case Blend::Source1Alpha:
                return D3D12_BLEND::D3D12_BLEND_SRC1_ALPHA;
            case Blend::InverseSource1Alpha:
                return D3D12_BLEND::D3D12_BLEND_INV_SRC1_ALPHA;
            case Blend::AlphaFactor:
                return D3D12_BLEND::D3D12_BLEND_ALPHA_FACTOR;
            case Blend::InverseAlphaFactor:
                return D3D12_BLEND::D3D12_BLEND_INV_ALPHA_FACTOR;
            }
        }
    }
}
