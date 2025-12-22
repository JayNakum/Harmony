#pragma once

#include "common.h"
#include "directx.h"

#include "HeapEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::HeapType with D3D12_HEAP_TYPE
        inline static D3D12_HEAP_TYPE toD3DHeapType(const HeapType e)
        {
            switch (e)
            {
            default:
            case HeapType::Default:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
            case HeapType::Upload:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
            case HeapType::Readback:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK;
            case HeapType::GPUUpload:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_GPU_UPLOAD;
            case HeapType::Custom:
                return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_CUSTOM;
            }
        }
    }

    namespace dx12
    {
        // Maps graphics::QueryHeapType with D3D12_QUERY_HEAP_TYPE
        inline static D3D12_QUERY_HEAP_TYPE toD3DQueryHeapType(const QueryHeapType e)
        {
            switch (e)
            {
            default:
                return static_cast<D3D12_QUERY_HEAP_TYPE>(-1);
            case QueryHeapType::Occlusion:
                return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_OCCLUSION;
            case QueryHeapType::CopyTimestamp:
                return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_COPY_QUEUE_TIMESTAMP;
            case QueryHeapType::ComputeTimestamp:
            case QueryHeapType::GraphicsTimestamp:
                return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
            case QueryHeapType::PipelineStats:
                return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS1;
            case QueryHeapType::StreamOutputStats:
                return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_SO_STATISTICS;
            case QueryHeapType::VideoDecodeStats:
                return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_VIDEO_DECODE_STATISTICS;
            }
        }
    }
}
