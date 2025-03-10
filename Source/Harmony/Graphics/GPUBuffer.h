#pragma once

#include "common.h"

#include "GPUResource.h"
#include "DescriptorHandle.h"

namespace graphics
{
    struct VertexBuffer
    {
        uint64_t bufferLocation;
        size_t sizeInBytes;
        size_t strideInBytes;
    };

    struct IndexBuffer
    {
        uint64_t bufferLocation;
        size_t sizeInBytes;
        bool is32Bit = false;
    };

    namespace dx12
    {
        class UploadBuffer;
        class Heap;


        class GPUBuffer : public GPUResource
        {
        protected:
            GPUBuffer()
                : mBufferSize(0), mElementCount(0), mElementSize(0),
                mResourceFlags(GPUResourceFlags::None) {}

        public:
            struct Description
            {
                std::string name;
                uint32_t elementCount;
                uint32_t elementSize;
            };

            inline const size_t getSize() const { return mBufferSize; }
            inline const uint32_t getElementCount() const { return mElementCount; }
            inline const uint32_t getElementSize() const { return mElementSize; }

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

        public:
            inline const DescriptorHandle& getSRV() const { return mSRV; }

            inline uint64_t rootConstantBufferView() const { return getGPUVirtualAddress(); }

            DescriptorHandle createConstantBufferView(uint32_t offset, size_t size) const;

            const VertexBuffer vertexBufferView(size_t offset, size_t size, size_t stride) const;
            inline const VertexBuffer vertexBufferView(size_t baseVertexIndex = 0) const
            {
                size_t offset = baseVertexIndex * mElementSize;
                return vertexBufferView(offset, (uint32_t)(mBufferSize - offset), mElementSize);
            }

            const IndexBuffer indexBufferView(size_t offset, size_t size, bool is32Bit = false) const;
            inline const IndexBuffer indexBufferView(size_t startIndex = 0) const
            {
                size_t offset = startIndex * mElementSize;
                return indexBufferView(offset, (uint32_t)(mBufferSize - offset), mElementSize == 4);
            }

        protected:
            // Creates a D3D12_RESOURCE_DESC based on the initData
            D3D12_RESOURCE_DESC describeBuffer() const;

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
            GPUResourceFlags mResourceFlags;
        };
    }

    namespace dx12
    {
        class ByteAddressBuffer : public GPUBuffer
        {
        public:
            virtual void createViews() override;
        };
    }
}
