#include "GPUBuffer.h"

#include "Log.h"
#include "Maths.h"

namespace graphics
{
    namespace dx12
    {
        GPUBuffer::GPUBuffer(const Description& createDescription)
            : mDescription(createDescription)
        {
            destroy();
            mUsageState = GPUResourceStates::CopyDestination; // Since the resource is created on default heap, it expects some data to be copied to.
        }

        const GPUResource::Description GPUBuffer::describeResource() const
        {
            GPUResource::Description description{};
            description.width = mDescription.width;
            description.dimension = GPUResourceDimension::Buffer;
            return description;
        }

        void GPUBuffer::createConstantBufferView(Device& device, DescriptorHandle& cbvHandle) const
        {
            size_t size = maths::alignUp(mDescription.width, 16);

            createConstantBufferView(device, cbvHandle, 0, size);
        }

        void GPUBuffer::createConstantBufferView(Device& device, DescriptorHandle& cbvHandle, size_t size, const uint32_t offset) const
        {
            HASSERT(offset + size <= mDescription.width, "offset(%d) + size(%d) is more than total buffer size(%d).", offset, size, mDescription.width);

            size = maths::alignUp(size, 16);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = getGPUVirtualAddress() + (size_t)offset;
            cbvDesc.SizeInBytes = (UINT)size;

            device->CreateConstantBufferView(&cbvDesc, cbvHandle);
        }

        const BufferView GPUBuffer::vertexBufferView(const size_t stride, const size_t size, const size_t offset) const
        {
            BufferView bufferView{};

            bufferView.bufferLocation = getGPUVirtualAddress() + offset;
            bufferView.strideInBytes = stride;
            bufferView.sizeInBytes = size;

            return bufferView;
        }

        const BufferView GPUBuffer::indexBufferView(const size_t size, const size_t offset, const bool is32Bits) const
        {
            BufferView bufferView{};

            bufferView.bufferLocation = getGPUVirtualAddress() + offset;
            bufferView.sizeInBytes = size;
            bufferView.is32Bit = is32Bits;

            return bufferView;
        }


        D3D12_VERTEX_BUFFER_VIEW BufferView::toD3DVertexBufferView(const BufferView& bufferView)
        {
            D3D12_VERTEX_BUFFER_VIEW d3dView{};
            d3dView.BufferLocation = bufferView.bufferLocation;
            d3dView.SizeInBytes = (UINT)bufferView.sizeInBytes;
            d3dView.StrideInBytes = (UINT)bufferView.strideInBytes;
            return d3dView;
        }

        D3D12_INDEX_BUFFER_VIEW BufferView::toD3DIndexBufferView(const BufferView& bufferView)
        {
            D3D12_INDEX_BUFFER_VIEW d3dView{};
            d3dView.BufferLocation = bufferView.bufferLocation;
            d3dView.SizeInBytes = (UINT)bufferView.sizeInBytes;
            d3dView.Format = toDXGIFormat(bufferView.is32Bit ? DefaultFormats::R32_UINT : DefaultFormats::R16_UINT);
            return d3dView;
        }
    }
}

namespace graphics
{
    namespace dx12
    {
        void ByteAddressBuffer::createShaderResourceView(Device& device, DescriptorHandle& srvHandle) const
        {
            HASSERT(maths::isAligned(mDescription.width, ByteAddressBuffer::DEFAULT_ALIGNMENT), "ByteAddressBuffer size must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);
            createShaderResourceView(device, srvHandle, mDescription.width);
        }

        void ByteAddressBuffer::createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const uint64_t sizeInBytes, const unsigned int offsetInBytes) const
        {
            HASSERT(offsetInBytes + sizeInBytes <= mDescription.width, "offset(%d) + size(%d) is more than total buffer size(%d).", offsetInBytes, sizeInBytes, mDescription.width);
            HASSERT(maths::isAligned(offsetInBytes, ByteAddressBuffer::DEFAULT_ALIGNMENT), "Byte offset must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);
            HASSERT(maths::isAligned(sizeInBytes, ByteAddressBuffer::DEFAULT_ALIGNMENT), "Byte size must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            // GPU SRV views into a ByteAddressBuffer are in units of 4 bytes.
            srvDesc.Buffer.FirstElement = (UINT)offsetInBytes / 4;
            srvDesc.Buffer.NumElements = (UINT)sizeInBytes / 4;

            srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

            device->CreateShaderResourceView(mResource.Get(), &srvDesc, srvHandle);
        }
    }
}
