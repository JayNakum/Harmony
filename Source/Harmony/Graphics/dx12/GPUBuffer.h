#pragma once

#include "common.h"

#include "GPUResource.h"
#include "DescriptorHandle.h"
#include "Heap.h"

#include "UploadBuffer.h"

namespace graphics
{
    namespace dx12
    {
        struct BufferView
        {
            uint64_t bufferLocation;
            size_t sizeInBytes;
            union
            {
                size_t strideInBytes;
                size_t elementSizeInBytes = 32;
            };
        };

        /*
        * The class represents a buffer on GPU Memory
        */
        class GPUBuffer : public GPUResource
        {
        protected:
            GPUBuffer(const Description& createDescription);

        public:
            virtual ~GPUBuffer()
            {
                destroy();
            }

        public:
            // Creates a filtered and valid GPUResource::Description based on the initData
            virtual const GPUResource::Description describeResource() const override;

            // For direct root32Consts
            inline const uint64_t rootConstantBufferView() const { return getGPUVirtualAddress(); }

            // Creates a CBV based on buffer description.
            // Requires a pre-allocated descriptorHandle& from a descriptor heap.
            void createConstantBufferView(DescriptorHandle& cbvHandle, uint32_t offset, size_t size) const;

            // Creates an vertex buffer view with custom offset, size and stride
            const BufferView vertexBufferView(size_t stride, size_t size, size_t offset = 0) const;

            // Creates a vertex buffer view of the entire buffer with stride = elementSizeInBytes.
            inline const BufferView vertexBufferView(size_t stride, size_t baseVertexIndex = 0) const
            {
                size_t offset = baseVertexIndex * stride;
                return vertexBufferView(stride, (uint32_t)(mDescription.width - offset), offset);
            }

            // Creates an index buffer view with custom offset, size and bits
            const BufferView indexBufferView(size_t elementSizeInBytes, size_t size, size_t offset = 0) const;

            // Creates an index buffer view of the entire buffer.
            inline const BufferView indexBufferView(size_t elementSizeInBytes, size_t startIndex = 0) const
            {
                size_t offset = startIndex * elementSizeInBytes;
                return indexBufferView(elementSizeInBytes, (uint32_t)(mDescription.width - offset), offset);
            }

        protected:
            const Description mDescription;
        };
    }
}

namespace graphics
{
    namespace dx12
    {
        /*
        * A ByteAddressBuffer is accessed per byte.
        * Dividing by 4 because GPU SRV views into a ByteAddressBuffer are in units of 4 bytes.
        */
        class ByteAddressBuffer : public GPUBuffer
        {
            static constexpr unsigned int DEFAULT_ALIGNMENT = 4;
        public:
            ByteAddressBuffer(const Description& createDescription)
                : GPUBuffer(createDescription) {}

            // Creates an SRV of the entire buffer.
            // Requires a pre allocated descriptorHandle& from a descriptor heap.
            inline void createShaderResourceView(DescriptorHandle& srvHandle) const
            {
                HASSERT(maths::isAligned(mDescription.width, ByteAddressBuffer::DEFAULT_ALIGNMENT), "ByteAddressBuffer size must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);
                createShaderResourceView(srvHandle, 0, mDescription.width);
            }

            // Creates an SRV with custom size and offset.
            // Requires a pre allocated descriptorHandle& from a descriptor heap.
            void createShaderResourceView(DescriptorHandle& srvHandle, unsigned int sizeInBytes, unsigned int offsetInBytes = 0) const;
        };
    }
}
