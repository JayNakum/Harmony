#pragma once

#include "common.h"

#include "UploadBuffer.h"

#include "dx12/GPUResource.h"
#include "dx12/DescriptorHandle.h"
#include "dx12/Heap.h"

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

    struct ConstantBuffer
    {
        uint64_t bufferLocation;
        size_t sizeInBytes;
    };

    class GPUMemory : public dx12::GPUResource
    {
    protected:
        GPUMemory()
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

        virtual ~GPUMemory()
        {
            destroy();
        }

        // Create a buffer.
        // If initial data is provided, it will be copied into the buffer using the default command context.
        void createCommitted(const Description& createDescription, const void* initialData = nullptr);

        // Create a buffer.
        // Data from the sourceUploadBuffer will be copied into the buffer using the default command context.
        void createCommitted(const Description& createDescription, UploadBuffer& sourceData, uint32_t sourceOffset = 0);

        // Sub-Allocate a buffer out of a pre-allocated heap.
        // If initial data is provided, it will be copied into the buffer using the default command context.
        void createPlaced(const Description& createDescription, dx12::Heap& backingHeap, uint64_t heapOffset, const void* initialData = nullptr);

        // Sub-Allocate a buffer out of a pre-allocated heap.
        // Data from the sourceUploadBuffer will be copied into the buffer using the default command context.
        void createPlaced(const Description& createDescription, dx12::Heap& backingHeap, uint64_t heapOffset, UploadBuffer& sourceData, uint32_t sourceOffset = 0);

    public:
        inline uint64_t rootConstantBufferView() const { return getGPUVirtualAddress(); }

        void createConstantBufferView(uint32_t offset, size_t size, dx12::DescriptorHandle& cbvHandle) const;

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
        // Creates a GPUResource::Description based on the initData
        dx12::GPUResource::Description describeBuffer() const;

        virtual void createViews(dx12::DescriptorHandle& srvHandle) = 0;

    private:
        // Initializes the local variables.
        // This is called inside the create method.
        void initialize(const Description& createDescription);

    protected:
        size_t mBufferSize;
        uint32_t mElementCount;
        uint32_t mElementSize;
        GPUResourceFlags mResourceFlags;
    };
}

namespace graphics
{
    class ByteAddressBuffer : public GPUMemory
    {
    public:
        virtual void createViews(dx12::DescriptorHandle& srvHandle) override;
    };
}
