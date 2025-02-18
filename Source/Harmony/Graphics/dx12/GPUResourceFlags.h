#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

namespace graphics
{
	namespace dx12
	{
		static const std::unordered_map<graphics::GPUResourceFlags, D3D12_RESOURCE_FLAGS> gpuResourceFlags = {
			{ GPUResourceFlags::None, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE },
			{ GPUResourceFlags::AllowRenderTarget, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET },
			{ GPUResourceFlags::AllowDepthStencil, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL },
			{ GPUResourceFlags::AllowUnorderedAccess, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS }
			// TODO: Add support for the following flags
			// { GPUResourceFlags::DenyShaderResource, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE },
			// { GPUResourceFlags::AllowCrossAdapter, D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER },
			// { GPUResourceFlags::AllowSimultaneousAccess, D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS },
			// { GPUResourceFlags::VideoDecodeReferenceOnly, D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY },
			// { GPUResourceFlags::VideoEncodeReferenceOnly, D3D12_RESOURCE_FLAG_VIDEO_ENCODE_REFERENCE_ONLY },
			// { GPUResourceFlags::RayTracingAccelerationStructure, D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE }
		};
	}
}