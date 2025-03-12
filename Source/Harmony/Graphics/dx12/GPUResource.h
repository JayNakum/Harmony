#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"
#include "GPUResourceStates.h"
#include "GPUResourceFlags.h"

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
                D3D12_RESOURCE_DIMENSION dimension;
                uint64_t alignment;
                uint64_t width;
                unsigned int height;
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

        protected:
            Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
            GPUResourceState mUsageState;
            GPUResourceState mTransitioningState;
            D3D12_GPU_VIRTUAL_ADDRESS mGPUVirtualAddress;
        };
    }
}
