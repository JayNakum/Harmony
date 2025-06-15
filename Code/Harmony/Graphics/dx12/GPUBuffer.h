#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "Device.h"

#include "GPUResource.h"
#include "DescriptorHandle.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * A common view for Vertex and Index buffer
        */
        struct BufferView
        {
            uint64_t bufferLocation;
            size_t sizeInBytes;
            union
            {
                size_t strideInBytes;
                bool is32Bit = false;
            };

            static D3D12_VERTEX_BUFFER_VIEW toD3DVertexBufferView(const BufferView& bufferView);
            static D3D12_INDEX_BUFFER_VIEW toD3DIndexBufferView(const BufferView& bufferView);
        };

        /*
        * The class represents a buffer on GPU Memory
        */
        class GPUBuffer : public GPUResource
        {
        protected:
            GPUBuffer(const Description& createDescription);

        public:
            // For direct root32Consts
            inline const uint64_t rootConstantBufferView() const { return getGPUVirtualAddress(); }

            // Creates a CBV based on buffer description.
            // Requires a pre-allocated descriptorHandle& from a descriptor heap.
            void createConstantBufferView(Device& device, DescriptorHandle& cbvHandle) const;
            void createConstantBufferView(Device& device, DescriptorHandle& cbvHandle, size_t size, const uint32_t offset = 0) const;

            // Creates an vertex buffer view with custom offset, size and stride
            const BufferView vertexBufferView(const size_t stride, const size_t size, const size_t offset = 0) const;

            // Creates a vertex buffer view of the entire buffer with stride = elementSizeInBytes.
            inline const BufferView vertexBufferView(const size_t stride, const size_t baseVertexIndex = 0) const
            {
                size_t offset = baseVertexIndex * stride;
                return vertexBufferView(stride, (uint32_t)(mDescription.width - offset), offset);
            }

            // Creates an index buffer view with custom offset, size and bits
            const BufferView indexBufferView(const size_t size, const size_t offset = 0, const bool is32Bits = false) const;

            // Creates an index buffer view of the entire buffer.
            inline const BufferView indexBufferView(const size_t startIndex = 0, const bool is32Bits = false) const
            {
                size_t offset = startIndex * (is32Bits ? sizeof(uint32_t) : sizeof(uint16_t));
                return indexBufferView((uint32_t)(mDescription.width - offset), offset, is32Bits);
            }

            virtual ~GPUBuffer() = default;
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

            void createShaderResourceView(Device& device, DescriptorHandle& srvHandle) const;
            // Creates an SRV with custom size and offset.
            // Requires a pre-allocated descriptorHandle& from a descriptor heap.
            void createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const uint64_t sizeInBytes, const unsigned int offsetInBytes = 0) const;

            void createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle) const;
            // Creates an UAV with custom size and offset.
            // Requires a pre-allocated descriptorHandle& from a descriptor heap.
            void createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle, const uint64_t sizeInBytes, const unsigned int offsetInBytes = 0) const;

            virtual ~ByteAddressBuffer() = default;
        };
    }
}

namespace graphics
{
    namespace dx12
    {
        /*
        * A buffer resource in UPLOAD_HEAP.
        */
        class UploadBuffer : public ByteAddressBuffer
        {
        public:
            // Creates a committed buffer of size Description::bufferSize
            UploadBuffer(Device& device, const Description& createDescription);

            // Maps the ID3D12Resource and returns the memory pointer.
            void* map();

            // Unmaps the resource from begin to end/mBufferSize
            void unmap(const size_t begin = 0, const size_t end = -1);

            // Calls destroy which frees the ComPtr<ID3D12Resource>
            ~UploadBuffer() = default;
        };
    }
}
