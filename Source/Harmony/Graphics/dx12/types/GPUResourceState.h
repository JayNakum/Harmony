#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

namespace graphics
{
    namespace dx12
    {
        inline static const D3D12_RESOURCE_STATES toD3DResourceState(const GPUResourceState state)
        {
            switch (state)
            {
            case GPUResourceState::Invalid:
                return static_cast<D3D12_RESOURCE_STATES>(-1);
            case GPUResourceState::Common:
                return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
            case GPUResourceState::CopyDest:
                return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST;
            case GPUResourceState::GenericRead:
                return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
                // TODO: Add support for more states as needed
            }
        }
    }
}
