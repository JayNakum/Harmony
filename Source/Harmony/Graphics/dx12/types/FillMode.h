#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::FillMode with D3D12_FILL_MODE
        inline static const D3D12_FILL_MODE toD3DFillMode(const FillMode e)
        {
            switch (e)
            {
            case FillMode::Solid:
                return D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
            case FillMode::Wireframe:
                return D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;
            }
        }
    }
}
