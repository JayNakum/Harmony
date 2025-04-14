#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::CullMode with D3D12_CULL_MODE
        inline static const D3D12_CULL_MODE toD3DCullMode(const CullMode e)
        {
            switch (e)
            {
            case CullMode::None:
                return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
            case CullMode::Front:
                return D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;
            case CullMode::Back:
                return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
            }
        }
    }
}
