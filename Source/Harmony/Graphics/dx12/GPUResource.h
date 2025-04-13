#pragma once

#include "common.h"
#include "directx.h"

#include "GPUResourceEnums.h"
#include "types/GPUResourceState.h"
#include "types/GPUResourceFlags.h"
#include "types/GPUResourceDimension.h"
#include "types/DefaultFormats.h"

namespace graphics
{
    namespace dx12
    {
        class Heap;

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
                uint64_t width;
                uint64_t height = 1;
                uint64_t depth = 1;
                uint16_t mipLevels = 1;
                GPUResourceDimension dimension;
                Format format = DefaultFormats::UNKNOWN;
                GPUResourceFlags resourceFlags = GPUResourceFlags::None;
                uint64_t alignment = 0;
                unsigned int sampleCount = 1;
                unsigned int sampleQuality = 0;
            };
            GPUResource()
                : mResource(nullptr),
                mUsageState(GPUResourceState::Common),
                mTransitioningState(GPUResourceState::Invalid) {}

            GPUResource(ID3D12Resource* resource, const GPUResourceState currentUsageState)
                : mResource(resource),
                mUsageState(currentUsageState),
                mTransitioningState(GPUResourceState::Invalid)
            {}

            ~GPUResource()
            {
                destroy();
            }

            static constexpr D3D12_GPU_VIRTUAL_ADDRESS NULL_ADDRESS = (D3D12_GPU_VIRTUAL_ADDRESS)0;
            static constexpr D3D12_GPU_VIRTUAL_ADDRESS INVALID_ADDRESS = (D3D12_GPU_VIRTUAL_ADDRESS)-1;


            // Create a committed resource.
            void createCommitted();

            // Sub-Allocate a buffer out of a pre-allocated heap.
            void createPlaced(Heap& backingHeap, uint64_t heapOffset);

            inline const GPUResourceState getUsageState() const { return mUsageState; }
            inline void setUsageState(const GPUResourceState newState) { mUsageState = newState; }

            inline const GPUResourceState getTransitioningState() const { return mTransitioningState; }
            inline void setTransitioningState(const GPUResourceState newState) { mTransitioningState = newState; }

            // Sets the ComPtr to null
            virtual void destroy()
            {
                mResource = nullptr;
            }

            // Operator overload that returns the raw pointer to ID3D12Resource
            const ID3D12Resource* operator->() const { return mResource.Get(); }
            ID3D12Resource* operator->() { return mResource.Get(); }

            // Returns the raw pointer to ID3D12Resource
            const ID3D12Resource* get() const { return mResource.Get(); }
            ID3D12Resource* get() { return mResource.Get(); }

            // Returns ComPtr::GetAddressOf
            ID3D12Resource** getAddressOf() { return mResource.GetAddressOf(); }

            const D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress() const { return mResource->GetGPUVirtualAddress(); }

            static const D3D12_RESOURCE_DESC toD3DResourceDesc(const Description& description);

            virtual const GPUResource::Description describeResource() const { return Description(); }

        protected:
            Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
            GPUResourceState mUsageState;
            GPUResourceState mTransitioningState;
        };
    }
}
