#pragma once

#include "common.h"
#include "directx.h"

#include "Device.h"

#include "GPUResourceEnums.h"

#include "Heap.h"

#include "types/GPUResourceStates.h"
#include "types/GPUResourceFlags.h"
#include "types/GPUResourceDimension.h"
#include "types/DefaultFormats.h"

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
            struct Description
            {
                std::string name;
                size_t width = 0;
                size_t height = 1;
                uint16_t depth = 1;
                uint16_t mipLevels = 1;
                GPUResourceDimension dimension = GPUResourceDimension::Unknown;
                Format format = DefaultFormats::UNKNOWN;
                GPUResourceFlags resourceFlags = GPUResourceFlags::None;
                GPUResourceStates initialState = GPUResourceStates::Common;
                float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                uint64_t alignment = 0;
                unsigned int sampleCount = 1;
                unsigned int sampleQuality = 0;
            };

            GPUResource(const GPUResourceStates initialState)
                : mResource(nullptr),
                mUsageState(initialState),
                mTransitioningState(GPUResourceStates::Common) {}

            void setResource(ID3D12Resource* resource)
            {
                mResource = resource;
            }

            // Create a committed resource.
            void createCommitted(Device& device);

            // Sub-Allocate a buffer out of a pre-allocated heap.
            void createPlaced(Device& device, Heap& backingHeap, uint64_t heapOffset);

            inline const GPUResourceStates getUsageState() const { return mUsageState; }
            inline void setUsageState(const GPUResourceStates newState) { mUsageState = newState; }

            inline const GPUResourceStates getTransitioningState() const { return mTransitioningState; }
            inline void setTransitioningState(const GPUResourceStates newState) { mTransitioningState = newState; }

            inline const D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress() const { return mResource->GetGPUVirtualAddress(); }

            virtual const GPUResource::Description describeResource() const { return Description(); }

            // Sets the ComPtr to null
            virtual void destroy()
            {
                if (mResource)
                {
                    mResource = nullptr;
                }
            }

            ~GPUResource()
            {
            }

            // Returns ComPtr::GetAddressOf().
            ID3D12Resource** getAddressOf() { return mResource.GetAddressOf(); }

            // Returns the raw pointer to ID3D12Resource.
            const ID3D12Resource* get() const { return mResource.Get(); }
            ID3D12Resource* get() { return mResource.Get(); }

            // Operator overload that returns the raw pointer to ID3D12Resource.
            const ID3D12Resource* operator->() const { return mResource.Get(); }
            ID3D12Resource* operator->() { return mResource.Get(); }

        public:
            static const D3D12_RESOURCE_DESC toD3DResourceDesc(const Description& description);

        protected:
            Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
            GPUResourceStates mUsageState;
            GPUResourceStates mTransitioningState;
        };
    }
}
