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

        GPUResource(const Description& description)
            : mResource(nullptr),
            mDescription(description),
            mHeapOffset(0),
            mUsageState(description.initialState),
            mTransitioningState(GPUResourceStates::Common) {
        }

        void setResource(ID3D12Resource* resource)
        {
            mResource = resource;
        }

        inline const Description& getDescription() const { return mDescription; }

        struct AllocationInfo
        {
            uint64_t sizeInBytes = 0;
            uint64_t alignment = 0;
        };
        inline AllocationInfo getAllocationInfo(Device& device) const
        {
            const D3D12_RESOURCE_DESC d3dDescription = GPUResource::toD3DResourceDesc(mDescription);
            const D3D12_RESOURCE_ALLOCATION_INFO d3dAllocationInfo = device->GetResourceAllocationInfo(0, 1, &d3dDescription);
            return AllocationInfo{ d3dAllocationInfo.SizeInBytes, d3dAllocationInfo.Alignment };
        }

        // Create a committed resource.
        void createCommitted(Device& device, const HeapType heapType = HeapType::Default);

        // Sub-Allocate a buffer out of a pre-allocated heap.
        void createPlaced(Device& device, Heap& backingHeap, uint64_t heapOffset);
        inline uint64_t getHeapOffset() const { return mHeapOffset; }

        // Maps the ID3D12Resource and returns the memory pointer.
        void* map(Device& device);

        // Unmaps the resource from begin to end/mBufferSize
        void unmap(const size_t begin = 0, size_t end = static_cast<size_t>(-1));

        inline GPUResourceStates getUsageState() const { return mUsageState; }
        inline void setUsageState(const GPUResourceStates newState) { mUsageState = newState; }

        inline GPUResourceStates getTransitioningState() const { return mTransitioningState; }
        inline void setTransitioningState(const GPUResourceStates newState) { mTransitioningState = newState; }

        inline D3D12_GPU_VIRTUAL_ADDRESS getGPUVirtualAddress() const { return mResource->GetGPUVirtualAddress(); }

        GPUResource(const GPUResource& copy);
        virtual ~GPUResource() = default;

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
        Description mDescription;
        uint64_t mHeapOffset = 0;
        GPUResourceStates mUsageState;
        GPUResourceStates mTransitioningState;
    };
}
