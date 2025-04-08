#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"
#include "types/GPUResourceStates.h"
#include "types/GPUResourceFlags.h"
#include "types/GPUResourceDimension.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3D12Resource
        * Every GPU Resource inherits this class.
        */
        class GPUResource
        {
        public:
            static constexpr D3D12_GPU_VIRTUAL_ADDRESS NULL_ADDRESS = (D3D12_GPU_VIRTUAL_ADDRESS)0;
            static constexpr D3D12_GPU_VIRTUAL_ADDRESS INVALID_ADDRESS = (D3D12_GPU_VIRTUAL_ADDRESS)-1;

            struct Description
            {
                uint64_t width;
                GPUResourceDimension dimension;
                GPUResourceFlags resourceFlags = GPUResourceFlags::None;
                uint64_t alignment = 0;
                unsigned int height = 1;
            };

            GPUResource()
                : mGPUVirtualAddress(NULL_ADDRESS),
                mUsageState(GPUResourceState::Common),
                mTransitioningState(GPUResourceState::Invalid) {}

            GPUResource(ID3D12Resource* resource, const GPUResourceState currentUsageState)
                : mGPUVirtualAddress(NULL_ADDRESS),
                mResource(resource),
                mUsageState(currentUsageState),
                mTransitioningState(GPUResourceState::Invalid) {}

            ~GPUResource()
            {
                destroy();
            }

            inline const GPUResourceState getUsageState() const { return mUsageState; }
            inline void setUsageState(const GPUResourceState newState) { mUsageState = newState; }

            inline const GPUResourceState getTransitioningState() const { return mTransitioningState; }
            inline void setTransitioningState(const GPUResourceState newState) { mTransitioningState = newState; }

            // Sets the ComPtr to null
            virtual void destroy()
            {
                mResource = nullptr;
                mGPUVirtualAddress = NULL_ADDRESS;
            }

            // Operator overload that returns the raw pointer to ID3D12Resource
            const ID3D12Resource* operator->() const { return mResource.Get(); }
            ID3D12Resource* operator->() { return mResource.Get(); }

            // Returns the raw pointer to ID3D12Resource
            const ID3D12Resource* get() const { return mResource.Get(); }
            ID3D12Resource* get() { return mResource.Get(); }

            // Returns ComPtr::GetAddressOf
            ID3D12Resource** getAddressOf() { return mResource.GetAddressOf(); }

            D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress() const { return mGPUVirtualAddress; }

            static D3D12_RESOURCE_DESC toResourceDesc(const Description& description)
            {
                D3D12_RESOURCE_DESC createDesc{};
                createDesc.Alignment = description.alignment;
                createDesc.DepthOrArraySize = 1;
                createDesc.Dimension = toD3DResourceDimension(description.dimension);
                createDesc.Flags = toD3DResourceFlags(description.resourceFlags);
                createDesc.Format = DXGI_FORMAT_UNKNOWN;
                createDesc.Height = description.height;
                createDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
                createDesc.MipLevels = 1;
                createDesc.SampleDesc.Count = 1;
                createDesc.SampleDesc.Quality = 0;
                createDesc.Width = (UINT64)description.width;
                return createDesc;
            }

        protected:
            Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
            GPUResourceState mUsageState;
            GPUResourceState mTransitioningState;
            D3D12_GPU_VIRTUAL_ADDRESS mGPUVirtualAddress;
        };
    }
}
