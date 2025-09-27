#pragma once

#include "common.h"
#include "Log.h"

namespace graphics
{
    enum class HeapType
    {
        Default, // Local
        Upload, // Host visible
        Readback,
        GPUUpload,
        Custom
    };

    inline static const std::string toString(const HeapType e)
    {
        switch (e)
        {
        default:
            return "";
        case HeapType::Default:
            return "Default";
        case HeapType::Upload:
            return "Upload";
        case HeapType::Readback:
            return "Readback";
        case HeapType::GPUUpload:
            return "GPUUpload";
        case HeapType::Custom:
            return "Custom";
        }
    }

    enum class QueryHeapType
    {
        Occlusion,
        CopyTimestamp,
        ComputeTimestamp,
        GraphicsTimestamp,
        PipelineStats,
        StreamOutputStats,
        VideoDecodeStats,
    };

    inline static const std::string toString(const QueryHeapType e)
    {
        switch (e)
        {
        default:
            return "";
        case QueryHeapType::Occlusion:
            return "Occlusion";
        case QueryHeapType::CopyTimestamp:
            return "CopyTimestamp";
        case QueryHeapType::ComputeTimestamp:
            return "ComputeTimestamp";
        case QueryHeapType::GraphicsTimestamp:
            return "GraphicsTimestamp";
        case QueryHeapType::PipelineStats:
            return "PipelineStats";
        case QueryHeapType::StreamOutputStats:
            return "StreamOutputStats";
        case QueryHeapType::VideoDecodeStats:
            return "VideoDecodeStats";
        }
    }
}
