#include "GPUMemory.h"

#include "Renderer.h"
#include "CommandContext.h"

#include "Maths.h"

namespace graphics
{
    void GPUMemory::createCommitted(const Description& createDescription, const void* initialData)
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
            dx12::gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
        );
        HASSERT_HR(hResult, "Failed to create committed resource %s.", createDescription.name);

#if defined(RELEASE)
        (void)createDescription.name;
#else
        mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif

        mGPUVirtualAddress = mResource->GetGPUVirtualAddress();

        if (initialData)
        {
            CommandContext::initializeBuffer(*this, initialData, mBufferSize);
        }

        createViews();
    }

    void GPUMemory::createCommitted(const Description& createDescription, UploadBuffer& sourceData, uint32_t sourceOffset)
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
            dx12::gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
        );
        HASSERT_HR(hResult, "Failed to create committed resource %s.", createDescription.name);

#if defined(RELEASE)
        (void)createDescription.name;
#else
        mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif

        mGPUVirtualAddress = mResource->GetGPUVirtualAddress();

        CommandContext::initializeBuffer(*this, sourceData, sourceOffset, mBufferSize);

        createViews();
    }

    void GPUMemory::createPlaced(const Description& createDescription, dx12::Heap& backingHeap, uint64_t heapOffset, const void* initialData)
    {
        initialize(createDescription);

        auto resourceDescription = dx12::GPUResource::toResourceDesc(describeBuffer());

        HRESULT hResult = GfxDevice()->CreatePlacedResource(
            (backingHeap.get()), heapOffset, &resourceDescription,
            dx12::gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
        );
        HASSERT_HR(hResult, "Failed to create placed resource %s.", createDescription.name);

#if defined(RELEASE)
        (void)createDescription.name;
#else
        mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif

        mGPUVirtualAddress = mResource->GetGPUVirtualAddress();

        if (initialData)
        {
            CommandContext::initializeBuffer(*this, initialData, mBufferSize);
        }

        createViews();
    }

    void GPUMemory::createPlaced(const Description& createDescription, dx12::Heap& backingHeap, uint64_t heapOffset, UploadBuffer& sourceData, uint32_t sourceOffset)
    {
        initialize(createDescription);

        auto resourceDescription = dx12::GPUResource::toResourceDesc(describeBuffer());

        HRESULT hResult = GfxDevice()->CreatePlacedResource(
            (backingHeap.get()), heapOffset, &resourceDescription,
            dx12::gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
        );
        HASSERT_HR(hResult, "Failed to create placed resource %s.", createDescription.name);

#if defined(RELEASE)
        (void)createDescription.name;
#else
        mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif

        mGPUVirtualAddress = mResource->GetGPUVirtualAddress();

        CommandContext::initializeBuffer(*this, sourceData, sourceOffset, mBufferSize);

        createViews();
    }

    dx12::DescriptorHandle GPUMemory::createConstantBufferView(uint32_t offset, size_t size) const
    {
        HASSERT(offset + size <= mBufferSize, "offset(%d) + size(%d) is more than total buffer size(%d).", offset, size, mBufferSize);

        size = maths::alignUp(size, 16);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
        cbvDesc.BufferLocation = mGPUVirtualAddress + (size_t)offset;
        cbvDesc.SizeInBytes = (UINT)size;

        dx12::DescriptorHandle hCBV = GfxAllocateDescriptor();
        GfxDevice()->CreateConstantBufferView(&cbvDesc, hCBV);
        return hCBV;
    }

    const VertexBuffer GPUMemory::vertexBufferView(size_t offset, size_t size, size_t stride) const
    {
        VertexBuffer bufferView{};

        bufferView.bufferLocation = mGPUVirtualAddress + offset;
        bufferView.sizeInBytes = size;
        bufferView.strideInBytes = stride;

        return bufferView;
    }

    const IndexBuffer GPUMemory::indexBufferView(size_t offset, size_t size, bool is32Bit) const
    {
        IndexBuffer bufferView{};

        bufferView.bufferLocation = mGPUVirtualAddress + offset;
        bufferView.sizeInBytes = size;
        bufferView.is32Bit = is32Bit;

        return bufferView;
    }

    dx12::GPUResource::Description GPUMemory::describeBuffer() const
    {
        dx12::GPUResource::Description description{};
        description.width = mBufferSize;
        description.dimension = GPUResourceDimension::Buffer;
        description.resourceFlags = mResourceFlags;
        return description;
    }

    void GPUMemory::initialize(const Description& createDescription)
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
    void ByteAddressBuffer::createViews()
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = (UINT)mBufferSize / 4;
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

        if (mSRV.getGPUPtr() == GPUResource::INVALID_ADDRESS)
        {
            mSRV = GfxAllocateDescriptor();
        }
        GfxDevice()->CreateShaderResourceView(mResource.Get(), &srvDesc, mSRV);
    }
}
