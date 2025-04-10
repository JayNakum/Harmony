#include "GPUBuffer.h"

#include "Renderer.h"
#include "CommandContext.h"

namespace graphics
{
    namespace dx12
    {
        void GPUBuffer::createCommitted(const Description& createDescription)
        {
            initialize(createDescription);

            static Heap::Description description{};
            description.name = "temp default heap description";
            description.sizeInBytes = 0;
            description.type = HeapType::Default;
            static D3D12_HEAP_PROPERTIES heapProperties = Heap::toD3DHeapProperties(description);

            D3D12_RESOURCE_DESC resourceDescription = GPUResource::toD3DResourceDesc(describeBuffer());

            HRESULT hResult = GfxDevice()->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription,
                toD3DResourceState(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
            );
            HASSERT_HR(hResult, "Failed to create committed resource %s.", createDescription.name);

#if defined(RELEASE)
            (void)createDescription.name;
#else
            mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif

            mGPUVirtualAddress = mResource->GetGPUVirtualAddress();
        }

        void GPUBuffer::createPlaced(const Description& createDescription, Heap& backingHeap, uint64_t heapOffset)
        {
            initialize(createDescription);

            D3D12_RESOURCE_DESC resourceDescription = GPUResource::toD3DResourceDesc(describeBuffer());

            HRESULT hResult = GfxDevice()->CreatePlacedResource(
                (backingHeap.get()), heapOffset, &resourceDescription,
                toD3DResourceState(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
            );
            HASSERT_HR(hResult, "Failed to create placed resource %s.", createDescription.name);

#if defined(RELEASE)
            (void)createDescription.name;
#else
            mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif

            mGPUVirtualAddress = mResource->GetGPUVirtualAddress();
        }

        void GPUBuffer::createConstantBufferView(DescriptorHandle& cbvHandle, uint32_t offset, size_t size) const
        {
            HASSERT(offset + size <= mBufferSize, "offset(%d) + size(%d) is more than total buffer size(%d).", offset, size, mBufferSize);

            size = maths::alignUp(size, 16);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = mGPUVirtualAddress + (size_t)offset;
            cbvDesc.SizeInBytes = (UINT)size;

            GfxDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);
        }

        const BufferView GPUBuffer::vertexBufferView(size_t offset, size_t size, size_t stride) const
        {
            BufferView bufferView{};

            bufferView.bufferLocation = mGPUVirtualAddress + offset;
            bufferView.sizeInBytes = size;
            bufferView.strideInBytes = stride;

            return bufferView;
        }

        const BufferView GPUBuffer::indexBufferView(size_t offset, size_t size, size_t elementSizeInBytes) const
        {
            BufferView bufferView{};

            bufferView.bufferLocation = mGPUVirtualAddress + offset;
            bufferView.sizeInBytes = size;
            bufferView.elementSizeInBytes = elementSizeInBytes;

            return bufferView;
        }

        const GPUResource::Description GPUBuffer::describeBuffer() const
        {
            GPUResource::Description description{};
            description.width = mBufferSize;
            description.dimension = GPUResourceDimension::Buffer;
            description.resourceFlags = mResourceFlags;
            return description;
        }

        void GPUBuffer::initialize(const Description& createDescription)
        {
            destroy();
            mElementCount = createDescription.elementCount;
            mElementSizeInBytes = createDescription.elementSizeInBytes;
            mBufferSize = mElementCount * mElementSizeInBytes;
            mUsageState = GPUResourceState::Common;
        }
    }
}

namespace graphics
{
    namespace dx12
    {
        void ByteAddressBuffer::createShaderResourceView(DescriptorHandle& srvHandle, unsigned int sizeInBytes, unsigned int offsetInBytes) const
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
}
