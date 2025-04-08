#pragma once

#include "common.h"
#include "directx.h"

#include "PipelineStateEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_INPUT_CLASSIFICATION toD3DInputClassification(const InputClassification classification)
        {
            switch (classification)
            {
            case InputClassification::PerVertex:
                return D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            case InputClassification::PerInstance:
                return D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
            }
        }
    }
}
