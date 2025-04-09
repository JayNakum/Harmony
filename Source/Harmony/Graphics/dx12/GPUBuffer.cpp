#include "GPUBuffer.h"

#include "Renderer.h"
#include "CommandContext.h"

#include "Maths.h"

namespace graphics
{
    void GPUBuffer::createCommitted(const Description& createDescription)
    {
        initialize(createDescription);

        static dx12::Heap::Description description{};
        description.name = "temp default heap description";
        description.sizeInBytes = 0;
        description.type = HeapType::Default;
        static auto heapProperties = dx12::Heap::toHeapProperties(description);

        auto resourceDescription = dx12::GPUResource::toResourceDesc(describeBuffer());

        HRESULT hResult = GfxDevice()->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription,
            dx12::toD3DResourceState(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
        );
        HASSERT_HR(hResult, "Failed to create committed resource %s.", createDescription.name);

#if defined(RELEASE)
        (void)createDescription.name;
#else
        mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif

        mGPUVirtualAddress = mResource->GetGPUVirtualAddress();
    }

    void GPUBuffer::createPlaced(const Description& createDescription, dx12::Heap& backingHeap, uint64_t heapOffset)
    {
        initialize(createDescription);

        auto resourceDescription = dx12::GPUResource::toResourceDesc(describeBuffer());

        HRESULT hResult = GfxDevice()->CreatePlacedResource(
            (backingHeap.get()), heapOffset, &resourceDescription,
            dx12::toD3DResourceState(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
        );
        HASSERT_HR(hResult, "Failed to create placed resource %s.", createDescription.name);

#if defined(RELEASE)
        (void)createDescription.name;
#else
        mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif

        mGPUVirtualAddress = mResource->GetGPUVirtualAddress();
    }

    const ConstantBufferView GPUBuffer::createConstantBufferView(dx12::DescriptorHandle& cbvHandle, uint32_t offset, size_t size) const
    {
        HASSERT(offset + size <= mBufferSize, "offset(%d) + size(%d) is more than total buffer size(%d).", offset, size, mBufferSize);

        size = maths::alignUp(size, 16);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
        cbvDesc.BufferLocation = mGPUVirtualAddress + (size_t)offset;
        cbvDesc.SizeInBytes = (UINT)size;

        ConstantBufferView cbv{};
        cbv.bufferLocation = cbvDesc.BufferLocation;
        cbv.sizeInBytes = cbvDesc.SizeInBytes;

        GfxDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);

        return cbv;
    }

    const VertexBufferView GPUBuffer::vertexBufferView(size_t offset, size_t size, size_t stride) const
    {
        VertexBufferView bufferView{};

        bufferView.bufferLocation = mGPUVirtualAddress + offset;
        bufferView.sizeInBytes = size;
        bufferView.strideInBytes = stride;

        return bufferView;
    }

    const IndexBufferView GPUBuffer::indexBufferView(size_t offset, size_t size, bool is32Bit) const
    {
        IndexBufferView bufferView{};

        bufferView.bufferLocation = mGPUVirtualAddress + offset;
        bufferView.sizeInBytes = size;
        bufferView.is32Bit = is32Bit;

        return bufferView;
    }

    dx12::GPUResource::Description GPUBuffer::describeBuffer() const
    {
        dx12::GPUResource::Description description{};
        description.width = mBufferSize;
        description.dimension = GPUResourceDimension::Buffer;
        description.resourceFlags = mResourceFlags;
        return description;
    }

    void GPUBuffer::initialize(const Description& createDescription)
    {
        destroy();
        mElementCount = createDescription.elementCount;
        mElementSize = createDescription.elementSize;
        mBufferSize = mElementCount * mElementSize;
        mUsageState = GPUResourceState::Common;
    }
}

namespace graphics
{
    void ByteAddressBuffer::createShaderResourceView(dx12::DescriptorHandle& srvHandle, unsigned int sizeInBytes, unsigned int offsetInBytes) const
    {
        HASSERT(offsetInBytes + sizeInBytes <= mBufferSize, "offset(%d) + size(%d) is more than total buffer size(%d).", offsetInBytes, sizeInBytes, mBufferSize);
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
