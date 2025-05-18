#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

namespace graphics
{
    namespace dx12
    {
        // Maps graphics::GPUResourceStates with D3D12_RESOURCE_STATES
        inline static const D3D12_RESOURCE_STATES toD3DResourceStates(const GPUResourceStates e)
        {
            D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_COMMON;

            if (hasFlag(e, GPUResourceStates::Common))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
            }
            if (hasFlag(e, GPUResourceStates::VertexAndConstantBuffer))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            }
            if (hasFlag(e, GPUResourceStates::IndexBuffer))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDEX_BUFFER;
            }
            if (hasFlag(e, GPUResourceStates::RenderTarget))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
            }
            if (hasFlag(e, GPUResourceStates::UnorderedAccess))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            }
            if (hasFlag(e, GPUResourceStates::DepthWrite))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
            }
            if (hasFlag(e, GPUResourceStates::DepthRead))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_READ;
            }
            if (hasFlag(e, GPUResourceStates::NonPixelShaderResource))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            }
            if (hasFlag(e, GPUResourceStates::PixelShaderResource))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            }
            if (hasFlag(e, GPUResourceStates::StreamOut))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_STREAM_OUT;
            }
            if (hasFlag(e, GPUResourceStates::IndirectArgument))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
            }
            if (hasFlag(e, GPUResourceStates::CopyDestination))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST;
            }
            if (hasFlag(e, GPUResourceStates::CopySource))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE;
            }
            if (hasFlag(e, GPUResourceStates::ResolveDestination))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RESOLVE_DEST;
            }
            if (hasFlag(e, GPUResourceStates::ResolveSource))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
            }
            if (hasFlag(e, GPUResourceStates::ShadingRateResource))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
            }
            if (hasFlag(e, GPUResourceStates::GenericRead))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
            }
            if (hasFlag(e, GPUResourceStates::AllShaderResource))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
            }
            if (hasFlag(e, GPUResourceStates::Present))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
            }
            if (hasFlag(e, GPUResourceStates::Predication))
            {
                states |= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PREDICATION;
            }

            return states;
        }
    }
}
