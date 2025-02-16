#pragma once

#include "common.h"

#include "GPUResource.h"

namespace graphics
{
    namespace dx12
    {
        class UploadBuffer;
        class Heap;

        class GPUBuffer : public GPUResource
        {
        public:
            virtual ~GPUBuffer()
            {
                destroy();
            }

            // Create a buffer.
            // If initial data is provided, it will be copied into the buffer using the default command context.
            void createCommitted(const std::string& name, uint32_t numElements, size_t elementSize,
                                 const void* initialData = nullptr);

            // Create a buffer.
            // Data from the sourceUploadBuffer will be copied into the buffer using the default command context.
            void createCommitted(const std::string& name, uint32_t numElements, size_t elementSize,
                                 const UploadBuffer& sourceData, uint32_t sourceOffset = 0);
            
            // Sub-Allocate a buffer out of a pre-allocated heap.
            // If initial data is provided, it will be copied into the buffer using the default command context.
            void createPlaced(const std::string& name, Heap& backingHeap, uint64_t heapOffset, uint32_t numElements, size_t elementSize,
                              const void* initialData = nullptr);
            
            // Sub-Allocate a buffer out of a pre-allocated heap.
            // Data from the sourceUploadBuffer will be copied into the buffer using the default command context.
            void createPlaced(const std::string& name, Heap& backingHeap, uint64_t heapOffset, uint32_t numElements, size_t elementSize,
                              const UploadBuffer& sourceData, uint32_t sourceOffset = 0);

        };
    }
}
