#include "GPUMemory.h"

#include "Renderer.h"
#include "CommandContext.h"

#include "UploadBuffer.h"

#include "dx12/DescriptorHeap.h"
#include "dx12/Heap.h"

#include "Maths.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        void GPUMemory::createCommitted(const Description& createDescription, const void* initialData)
        {
            initialize(createDescription);

            static Heap::Description description{};
            description.name = "temp default heap description";
            description.sizeInBytes = 0;
            description.type = HeapType::Default;
            static auto heapProperties = Heap::toHeapProperties(description);

            D3D12_RESOURCE_DESC resourceDescription = describeBuffer();

            HRESULT hResult = GfxDevice()->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription,
                gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
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

        void GPUMemory::createCommitted(const Description& createDescription, const UploadBuffer& sourceData, uint32_t sourceOffset)
        {
            initialize(createDescription);

            static D3D12_HEAP_PROPERTIES heapProperties;
            heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProperties.CreationNodeMask = 1;
            heapProperties.VisibleNodeMask = 1;

            D3D12_RESOURCE_DESC resourceDescription = describeBuffer();

            HRESULT hResult = GfxDevice()->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription,
                gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
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

        void GPUMemory::createPlaced(const Description& createDescription, Heap& backingHeap, uint64_t heapOffset, const void* initialData)
        {
            initialize(createDescription);

            D3D12_RESOURCE_DESC resourceDescription = describeBuffer();

            HRESULT hResult = GfxDevice()->CreatePlacedResource(
                (backingHeap.get()), heapOffset, &resourceDescription,
                gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
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

        void GPUMemory::createPlaced(const Description& createDescription, Heap& backingHeap, uint64_t heapOffset, const UploadBuffer& sourceData, uint32_t sourceOffset)
        {
            initialize(createDescription);

            D3D12_RESOURCE_DESC resourceDescription = describeBuffer();

            HRESULT hResult = GfxDevice()->CreatePlacedResource(
                (backingHeap.get()), heapOffset, &resourceDescription,
                gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(getAddressOf())
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

        DescriptorHandle GPUMemory::createConstantBufferView(uint32_t offset, size_t size) const
        {
            HASSERT(offset + size <= mBufferSize, "offset(%d) + size(%d) is more than total buffer size(%d).", offset, size, mBufferSize);

            size = maths::alignUp(size, 16);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = mGPUVirtualAddress + (size_t)offset;
            cbvDesc.SizeInBytes = (UINT)size;

            DescriptorHandle hCBV = GfxAllocateDescriptor();
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

        D3D12_RESOURCE_DESC GPUMemory::describeBuffer() const
        {
            HASSERT(mBufferSize != 0, "Trying to create a buffer with size 0.");

            D3D12_RESOURCE_DESC createDesc{};
            createDesc.Alignment = 0;
            createDesc.DepthOrArraySize = 1;
            createDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            createDesc.Flags = gpuResourceFlags.at(mResourceFlags);
            createDesc.Format = DXGI_FORMAT_UNKNOWN;
            createDesc.Height = 1;
            createDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            createDesc.MipLevels = 1;
            createDesc.SampleDesc.Count = 1;
            createDesc.SampleDesc.Quality = 0;
            createDesc.Width = (UINT64)mBufferSize;
            return createDesc;
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

    namespace dx12
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
}
