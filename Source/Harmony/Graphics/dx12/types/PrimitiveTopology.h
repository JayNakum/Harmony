#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::PrimitiveTopology with D3D12_PRIMITIVE_TOPOLOGY
        inline static const D3D12_PRIMITIVE_TOPOLOGY toD3DPrimitiveTopology(const PrimitiveTopology e)
        {
            switch (e)
            {
            default:
            case PrimitiveTopology::Undefined:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
            case PrimitiveTopology::PointList:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
            case PrimitiveTopology::LineList:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
            case PrimitiveTopology::LineStrip:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
            case PrimitiveTopology::TriangleList:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case PrimitiveTopology::TriangleStrip:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            case PrimitiveTopology::LineListAdj:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
            case PrimitiveTopology::LineStripAdj:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
            case PrimitiveTopology::TriangleListAdj:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
            case PrimitiveTopology::TriangleStripAdj:
                return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
            }
        }

        // Maps graphics::PrimitiveTopologyType with D3D12_PRIMITIVE_TOPOLOGY
        inline static const D3D12_PRIMITIVE_TOPOLOGY_TYPE toD3DPrimitiveTopologyType(const PrimitiveTopologyType e)
        {
            switch (e)
            {
            default:
            case PrimitiveTopologyType::Undefined:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
            case PrimitiveTopologyType::Point:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            case PrimitiveTopologyType::Line:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            case PrimitiveTopologyType::Triangle:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            case PrimitiveTopologyType::Patch:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
            }
        }
    }
}
