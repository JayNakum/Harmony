#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"

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

            GPUResource()
                : mGPUVirtualAddress(NULL_ADDRESS),
                mUsageState(D3D12_RESOURCE_STATE_COMMON),
                mTransitioningState((D3D12_RESOURCE_STATES)-1) {}

            GPUResource(ID3D12Resource* resource, const D3D12_RESOURCE_STATES currentUsageState)
                : mGPUVirtualAddress(NULL_ADDRESS),
                mResource(resource),
                mUsageState(currentUsageState),
                mTransitioningState((D3D12_RESOURCE_STATES)-1) {}

            ~GPUResource()
            {
                destroy();
            }

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
            D3D12_RESOURCE_STATES mUsageState;
            D3D12_RESOURCE_STATES mTransitioningState;
            D3D12_GPU_VIRTUAL_ADDRESS mGPUVirtualAddress;
        };
    }
}
