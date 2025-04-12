#include "GPUBuffer.h"

#include "Renderer.h"
#include "CommandContext.h"

namespace graphics
{
    namespace dx12
    {
        GPUBuffer::GPUBuffer(const Description& createDescription)
            : mDescription(createDescription)
        {
            destroy();
            mUsageState = GPUResourceState::CopyDest; // Since the resource is created on default heap, it expects some data to be copied to.
        }

        void GPUBuffer::createConstantBufferView(DescriptorHandle& cbvHandle, uint32_t offset, size_t size) const
        {
            HASSERT(offset + size <= mDescription.width, "offset(%d) + size(%d) is more than total buffer size(%d).", offset, size, mDescription.width);

            size = maths::alignUp(size, 16);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = getGPUVirtualAddress() + (size_t)offset;
            cbvDesc.SizeInBytes = (UINT)size;

            GfxDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);
        }

        const BufferView GPUBuffer::vertexBufferView(size_t stride, size_t size, size_t offset) const
        {
            BufferView bufferView{};

            bufferView.bufferLocation = getGPUVirtualAddress() + offset;
            bufferView.strideInBytes = stride;
            bufferView.sizeInBytes = size;

            return bufferView;
        }

        const BufferView GPUBuffer::indexBufferView(size_t elementSizeInBytes, size_t size, size_t offset) const
        {
            BufferView bufferView{};

            bufferView.bufferLocation = getGPUVirtualAddress() + offset;
            bufferView.sizeInBytes = size;
            bufferView.elementSizeInBytes = elementSizeInBytes;

            return bufferView;
        }

        const GPUResource::Description GPUBuffer::describeResource() const
        {
            GPUResource::Description description{};
            description.width = mDescription.width;
            description.dimension = GPUResourceDimension::Buffer;
            return description;
        }
    }
}

namespace graphics
{
    namespace dx12
    {
        void ByteAddressBuffer::createShaderResourceView(DescriptorHandle& srvHandle, unsigned int sizeInBytes, unsigned int offsetInBytes) const
        {
            HASSERT(offsetInBytes + sizeInBytes <= mDescription.width, "offset(%d) + size(%d) is more than total buffer size(%d).", offsetInBytes, sizeInBytes, mDescription.width);
            HASSERT(maths::isAligned(offsetInBytes, ByteAddressBuffer::DEFAULT_ALIGNMENT), "Byte offset must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);
            HASSERT(maths::isAligned(sizeInBytes, ByteAddressBuffer::DEFAULT_ALIGNMENT), "Byte size must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            srvDesc.Buffer.FirstElement = offsetInBytes / 4;
            srvDesc.Buffer.NumElements = sizeInBytes / 4;

            srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

            GfxDevice()->CreateShaderResourceView(mResource.Get(), &srvDesc, srvHandle);
        }
    }
}
