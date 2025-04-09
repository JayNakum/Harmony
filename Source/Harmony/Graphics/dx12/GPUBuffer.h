#pragma once

#include "common.h"

#include "UploadBuffer.h"

#include "GPUResource.h"
#include "DescriptorHandle.h"
#include "Heap.h"

namespace graphics
{
    struct ConstantBufferView
    {
        uint64_t bufferLocation;
        size_t sizeInBytes;
    };

    struct VertexBufferView
    {
        uint64_t bufferLocation;
        size_t sizeInBytes;
        size_t strideInBytes;
    };

    struct IndexBufferView
    {
        uint64_t bufferLocation;
        size_t sizeInBytes;
        bool is32Bit = false;
    };

    class GPUBuffer : public dx12::GPUResource
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

        // Create a committed resource.
        void createCommitted(const Description& createDescription);

        // Sub-Allocate a buffer out of a pre-allocated heap.
        void createPlaced(const Description& createDescription, dx12::Heap& backingHeap, uint64_t heapOffset);

    public:
        // For direct root32Consts
        inline uint64_t rootConstantBufferView() const { return getGPUVirtualAddress(); }

        // Creates a CBV based on buffer description.
        // Requires a pre allocated descriptorHandle& from a descriptor heap.
        const ConstantBufferView createConstantBufferView(dx12::DescriptorHandle& cbvHandle, uint32_t offset, size_t size) const;

        // Creates an vertex buffer view with custom offset, size and stride
        const VertexBufferView vertexBufferView(size_t offset, size_t size, size_t stride) const;

        // Creates a vertex buffer view of the entire buffer.
        inline const VertexBufferView vertexBufferView(size_t baseVertexIndex = 0) const
        {
            size_t offset = baseVertexIndex * mElementSize;
            return vertexBufferView(offset, (uint32_t)(mBufferSize - offset), mElementSize);
        }

        // Creates an index buffer view with custom offset, size and bits
        const IndexBufferView indexBufferView(size_t offset, size_t size, bool is32Bit = false) const;

        // Creates an index buffer view of the entire buffer.
        inline const IndexBufferView indexBufferView(size_t startIndex = 0) const
        {
            size_t offset = startIndex * mElementSize;
            return indexBufferView(offset, (uint32_t)(mBufferSize - offset), mElementSize == 4);
        }

    protected:
        // Creates a GPUResource::Description based on the initData
        dx12::GPUResource::Description describeBuffer() const;

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
    /*
    * A ByteAddressBuffer is accessed per byte.
    * Dividing by 4 because GPU SRV views into a ByteAddressBuffer are in units of 4 bytes.
    */
    class ByteAddressBuffer : public GPUBuffer
    {
        static constexpr unsigned int DEFAULT_ALIGNMENT = 4;
    public:
        // Creates an SRV of the entire buffer.
        // Requires a pre allocated descriptorHandle& from a descriptor heap.
        inline void createShaderResourceView(dx12::DescriptorHandle& srvHandle) const
        {
            HASSERT(maths::isAligned(mBufferSize, ByteAddressBuffer::DEFAULT_ALIGNMENT), "ByteAddressBuffer size must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);
            createShaderResourceView(srvHandle, 0, mBufferSize);
        }

        // Creates an SRV with custom size and offset.
        // Requires a pre allocated descriptorHandle& from a descriptor heap.
        void createShaderResourceView(dx12::DescriptorHandle& srvHandle, unsigned int sizeInBytes, unsigned int offsetInBytes = 0) const;
    };
}
