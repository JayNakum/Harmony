#include "PipelineState.h"

namespace graphics
{
    namespace dx12
    {
        GraphicsPipelineState::GraphicsPipelineState(const Description& description)
            : PipelineState(description.name)
        {
            // TODO: Create graphics PSO
        }

        D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineState::toD3DDescription(const Description & description)
        {
            D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dDescription{};

            return d3dDescription;
        }
    }
}
