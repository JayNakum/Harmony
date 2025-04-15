#include "UploadBuffer.h"

#include "dx12/Heap.h"
#include "dx12/types/GPUResourceState.h"

namespace graphics
{
    namespace dx12
    {
        UploadBuffer::UploadBuffer(const Description& createDescription)
            : mBufferSize(createDescription.bufferSize)
        {
            destroy();

            // Create an upload buffer. This is CPU-visible, but it's write combined memory,
            // so avoid reading back from it.
            static Heap::Description description{};
            description.name = "temp upload heap description";
            description.sizeInBytes = 0;
            description.type = HeapType::Upload;
            static D3D12_HEAP_PROPERTIES heapProperties = Heap::toD3DHeapProperties(description);

            // Upload buffers must be 1-dimensional
            D3D12_RESOURCE_DESC createDesc = GPUResource::toD3DResourceDesc(describeResource());

            mUsageState = GPUResourceState::GenericRead;

            HRESULT hResult = GfxDevice()->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &createDesc,
                dx12::toD3DResourceState(mUsageState), nullptr, IID_PPV_ARGS(&mResource));

            HASSERT(SUCCEEDED(hResult), "Failed to create upload buffer %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            setName(mResource.Get(), utility::toWideString(createDescription.name).c_str());
        }

        const GPUResource::Description UploadBuffer::describeResource() const
        {
            GPUResource::Description description{};
            description.width = mBufferSize;
            description.dimension = GPUResourceDimension::Buffer;

            return description;
        }

        void* UploadBuffer::map()
        {
            void* memory = nullptr;
            HRESULT hResult = mResource->Map(0, &CD3DX12_RANGE(0, mBufferSize), &memory);
            HASSERT(SUCCEEDED(hResult), "Failed to map upload buffer resource.\n%s", utility::translateErrorCode(hResult));
            return memory;
        }

        void UploadBuffer::unmap(const size_t begin, const size_t end)
        {
            mResource->Unmap(0, &CD3DX12_RANGE(begin, std::min(end, mBufferSize)));
        }
    }
}
