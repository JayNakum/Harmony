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
                : mBufferSize(0), mElementCount(0), mElementSize(0) {}

        public:
            struct Description
            {
                std::string name;
                uint32_t elementCount;
                uint32_t elementSize;
            };

            virtual ~GPUBuffer()
            {
                destroy();
            }
            
            // Create a buffer.
            // If initial data is provided, it will be copied into the buffer using the default command context.
            void createCommitted(const Description& createDescription, const void* initialData = nullptr);

            // Create a buffer.
            // Data from the sourceUploadBuffer will be copied into the buffer using the default command context.
            void createCommitted(const Description& createDescription, const UploadBuffer& sourceData, uint32_t sourceOffset = 0);
            
            // Sub-Allocate a buffer out of a pre-allocated heap.
            // If initial data is provided, it will be copied into the buffer using the default command context.
            void createPlaced(const Description& createDescription, Heap& backingHeap, uint64_t heapOffset, const void* initialData = nullptr);
            
            // Sub-Allocate a buffer out of a pre-allocated heap.
            // Data from the sourceUploadBuffer will be copied into the buffer using the default command context.
            void createPlaced(const Description& createDescription, Heap& backingHeap, uint64_t heapOffset, const UploadBuffer& sourceData, uint32_t sourceOffset = 0);

        protected:
            // Creates a D3D12_RESOURCE_DESC based on the initData
            D3D12_RESOURCE_DESC describeBuffer();

            virtual void createViews() = 0;

        private:
			// Initializes the local variables.
			// This is called inside the create method.
			void initialize(const Description& createDescription);

        protected:
            DescriptorHandle mSRV;
            // TODO: DescriptorHandle mUAV;
            
            size_t mBufferSize;
            uint32_t mElementCount;
            uint32_t mElementSize;
        };
    }
}
