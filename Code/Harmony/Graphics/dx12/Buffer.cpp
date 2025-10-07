#include "Buffer.h"

namespace graphics
{
    Buffer::Buffer(const Description& createDescription)
        : GPUResource(createDescription)
    {
        HASSERT(createDescription.dimension == GPUResourceDimension::Buffer, "Invalid resource dimension for buffer.");
    }

    void Buffer::createConstantBufferView(Device& device, DescriptorHandle& cbvHandle) const
    {
        size_t size = maths::alignUp(mDescription.width, 16);

        createConstantBufferView(device, cbvHandle, size);
    }

    void Buffer::createConstantBufferView(Device& device, DescriptorHandle& cbvHandle, size_t size, const uint32_t offset) const
    {
        HASSERT(offset + size <= mDescription.width, "offset(%d) + size(%d) is more than total buffer size(%d).", offset, size, mDescription.width);

        size = maths::alignUp(size, 16);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
        cbvDesc.BufferLocation = getGPUVirtualAddress() + (size_t)offset;
        cbvDesc.SizeInBytes = (UINT)size;

        device->CreateConstantBufferView(&cbvDesc, cbvHandle);
    }

    const BufferView Buffer::vertexBufferView(const size_t stride, const size_t size, const size_t offset) const
    {
        BufferView bufferView{};

        bufferView.bufferLocation = getGPUVirtualAddress() + offset;
        bufferView.strideInBytes = stride;
        bufferView.sizeInBytes = size;

        return bufferView;
    }

    const BufferView Buffer::indexBufferView(const size_t size, const size_t offset, const bool is32Bits) const
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
        d3dView.Format = dx12::toDXGIFormat(bufferView.is32Bit ? DefaultFormats::R32_UINT : DefaultFormats::R16_UINT);
        return d3dView;
    }
}

namespace graphics
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
        srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        // GPU views into a ByteAddressBuffer are in units of 4 bytes.
        srvDesc.Buffer.FirstElement = (UINT)offsetInBytes / 4;
        srvDesc.Buffer.NumElements = (UINT)sizeInBytes / 4;

        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

        device->CreateShaderResourceView(mResource.Get(), &srvDesc, srvHandle);
    }

    void ByteAddressBuffer::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle) const
    {
        HASSERT(maths::isAligned(mDescription.width, ByteAddressBuffer::DEFAULT_ALIGNMENT), "ByteAddressBuffer size must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);
        createUnorderedAccessView(device, uavHandle, mDescription.width);
    }

    void ByteAddressBuffer::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle, const uint64_t sizeInBytes, const unsigned int offsetInBytes) const
    {
        HASSERT(offsetInBytes + sizeInBytes <= mDescription.width, "offset(%d) + size(%d) is more than total buffer size(%d).", offsetInBytes, sizeInBytes, mDescription.width);
        HASSERT(maths::isAligned(offsetInBytes, ByteAddressBuffer::DEFAULT_ALIGNMENT), "Byte offset must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);
        HASSERT(maths::isAligned(sizeInBytes, ByteAddressBuffer::DEFAULT_ALIGNMENT), "Byte size must be %d-byte aligned.", ByteAddressBuffer::DEFAULT_ALIGNMENT);

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

        // GPU views into a ByteAddressBuffer are in units of 4 bytes.
        uavDesc.Buffer.FirstElement = (UINT)offsetInBytes / 4;
        uavDesc.Buffer.NumElements = (UINT)sizeInBytes / 4;

        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

        device->CreateUnorderedAccessView(mResource.Get(), nullptr, &uavDesc, uavHandle);
    }
}

namespace graphics
{
    StructuredBuffer::StructuredBuffer(const Description& createDescription, const unsigned int elementCount, const unsigned int strideInBytes)
        : Buffer(createDescription), mElementCount(elementCount), mStrideInBytes(strideInBytes)
    {
        HASSERT(createDescription.width == (elementCount * strideInBytes), "Invalid resource width for structured buffer.");
    }

    void StructuredBuffer::createShaderResourceView(Device& device, DescriptorHandle& srvHandle) const
    {
        createShaderResourceView(device, srvHandle, mElementCount, mStrideInBytes);
    }

    void StructuredBuffer::createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset) const
    {
        HASSERT((elementCount + elementOffset) * strideInBytes <= mDescription.width, "offset(%d) + elementCount(%d) is more than total buffer size(%d).", elementOffset, elementCount, mDescription.width);
        DEBUGLOG_IF(strideInBytes != mStrideInBytes, "WARNING", "Provided stride(%d) doesn't match with creation stride(%d)", strideInBytes, mStrideInBytes);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        srvDesc.Buffer.FirstElement = (UINT)elementOffset;
        srvDesc.Buffer.NumElements = (UINT)elementCount;
        srvDesc.Buffer.StructureByteStride = (UINT)strideInBytes;

        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(mResource.Get(), &srvDesc, srvHandle);
    }

    void StructuredBuffer::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle) const
    {
        createUnorderedAccessView(device, uavHandle, mElementCount, mStrideInBytes);
    }

    void StructuredBuffer::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset) const
    {
        HASSERT((elementCount + elementOffset) * strideInBytes <= mDescription.width, "offset(%d) + elementCount(%d) is more than total buffer size(%d).", elementOffset, elementCount, mDescription.width);
        DEBUGLOG_IF(strideInBytes != mStrideInBytes, "WARNING", "Provided stride(%d) doesn't match with creation stride(%d)", strideInBytes, mStrideInBytes);

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

        uavDesc.Buffer.FirstElement = (UINT)elementOffset;
        uavDesc.Buffer.NumElements = (UINT)elementCount;
        uavDesc.Buffer.StructureByteStride = (UINT)strideInBytes;

        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        device->CreateUnorderedAccessView(mResource.Get(), nullptr, &uavDesc, uavHandle);
    }
}

namespace graphics
{
    TypedBuffer::TypedBuffer(const Description& createDescription, const unsigned int elementCount, const unsigned int strideInBytes)
        : Buffer(createDescription), mElementCount(elementCount), mStrideInBytes(strideInBytes)
    {
        HASSERT(createDescription.width == (elementCount * strideInBytes), "Invalid resource width for typed buffer.");
    }

    void TypedBuffer::createShaderResourceView(Device& device, DescriptorHandle& srvHandle) const
    {
        createShaderResourceView(device, srvHandle, mElementCount, mStrideInBytes);
    }

    void TypedBuffer::createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset) const
    {
        HASSERT((elementCount + elementOffset) * strideInBytes <= mDescription.width, "offset(%d) + elementCount(%d) is more than total buffer size(%d).", elementOffset, elementCount, mDescription.width);
        DEBUGLOG_IF(strideInBytes != mStrideInBytes, "WARNING", "Provided stride(%d) doesn't match with creation stride(%d)", strideInBytes, mStrideInBytes);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        srvDesc.Buffer.FirstElement = (UINT)elementOffset;
        srvDesc.Buffer.NumElements = (UINT)elementCount;
        srvDesc.Buffer.StructureByteStride = (UINT)strideInBytes;

        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(mResource.Get(), &srvDesc, srvHandle);
    }

    void TypedBuffer::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle) const
    {
        createUnorderedAccessView(device, uavHandle, mElementCount, mStrideInBytes);
    }

    void TypedBuffer::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset) const
    {
        HASSERT((elementCount + elementOffset) * strideInBytes <= mDescription.width, "offset(%d) + elementCount(%d) is more than total buffer size(%d).", elementOffset, elementCount, mDescription.width);
        DEBUGLOG_IF(strideInBytes != mStrideInBytes, "WARNING", "Provided stride(%d) doesn't match with creation stride(%d)", strideInBytes, mStrideInBytes);

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

        uavDesc.Buffer.FirstElement = (UINT)elementOffset;
        uavDesc.Buffer.NumElements = (UINT)elementCount;
        uavDesc.Buffer.StructureByteStride = (UINT)strideInBytes;

        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        device->CreateUnorderedAccessView(mResource.Get(), nullptr, &uavDesc, uavHandle);
    }
}
