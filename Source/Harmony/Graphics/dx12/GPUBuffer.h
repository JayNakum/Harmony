#pragma once

#include "common.h"

#include "GPUResource.h"
#include "DescriptorHandle.h"

namespace graphics
{
    namespace dx12
    {
        class UploadBuffer;
        class Heap;

        class GPUBuffer : public GPUResource
        {
        protected:
            GPUBuffer()
                : mBufferSize(0), mElementCount(0), mElementSize(0) 
            {

            }

        public:
            virtual ~GPUBuffer()
            {
                destroy();
            }

            // Create a buffer.
            // If initial data is provided, it will be copied into the buffer using the default command context.
            void createCommitted(const std::string& name, uint32_t numElements, uint32_t elementSize,
                                 const void* initialData = nullptr);

            // Create a buffer.
            // Data from the sourceUploadBuffer will be copied into the buffer using the default command context.
            void createCommitted(const std::string& name, uint32_t numElements, uint32_t elementSize,
                                 const UploadBuffer& sourceData, uint32_t sourceOffset = 0);
            
            // Sub-Allocate a buffer out of a pre-allocated heap.
            // If initial data is provided, it will be copied into the buffer using the default command context.
            void createPlaced(const std::string& name, Heap& backingHeap, uint64_t heapOffset, uint32_t numElements, uint32_t elementSize,
                              const void* initialData = nullptr);
            
            // Sub-Allocate a buffer out of a pre-allocated heap.
            // Data from the sourceUploadBuffer will be copied into the buffer using the default command context.
            void createPlaced(const std::string& name, Heap& backingHeap, uint64_t heapOffset, uint32_t numElements, uint32_t elementSize,
                              const UploadBuffer& sourceData, uint32_t sourceOffset = 0);

        protected:
            D3D12_RESOURCE_DESC describeBuffer();
            virtual void createViews() = 0;

        protected:
            DescriptorHandle mSRV;
            // TODO: DescriptorHandle mUAV;
            
            size_t mBufferSize;
            uint32_t mElementCount;
            uint32_t mElementSize;
        };
    }
}
