#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_PRIMITIVE_TOPOLOGY_TYPE toD3DPrimitiveTopologyType(const PrimitiveTopology topology)
        {
            switch (topology)
            {
            case PrimitiveTopology::Undefined:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
            case PrimitiveTopology::Point:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            case PrimitiveTopology::Line:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            case PrimitiveTopology::Triangle:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            case PrimitiveTopology::Patch:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
            }
        }
    }
}
