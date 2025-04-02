#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

namespace graphics
{
    namespace dx12
    {
        // maps graphics::GPUResourceState and D3D12_RESOURCE_STATES
        static const std::unordered_map<graphics::GPUResourceState, D3D12_RESOURCE_STATES> gpuResourceStates = {
            { GPUResourceState::Invalid, D3D12_RESOURCE_STATES(-1)},
            { GPUResourceState::Common, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON },
            { GPUResourceState::CopyDest, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST },
            { GPUResourceState::GenericRead, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ }
            // TODO: Add support for the more states as needed
        };
    }
}
