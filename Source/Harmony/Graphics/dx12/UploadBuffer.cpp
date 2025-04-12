#include "UploadBuffer.h"

#include "Renderer.h"

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

            HASSERT_HR(hResult, "Failed to create upload buffer %s", createDescription.name);

            mGPUVirtualAddress = mResource->GetGPUVirtualAddress();

#if defined(RELEASE)
            (void)(name);
#else
            mResource->SetName(utility::toWideString(createDescription.name).c_str());
#endif
        }

        void* UploadBuffer::map()
        {
            void* memory = nullptr;
            HRESULT hResult = mResource->Map(0, &CD3DX12_RANGE(0, mBufferSize), &memory);
            HASSERT_HR(hResult, "Failed to map upload buffer resource.");
            return memory;
        }

        void UploadBuffer::unmap(size_t begin, size_t end)
        {
            mResource->Unmap(0, &CD3DX12_RANGE(begin, std::min(end, mBufferSize)));
        }

        const GPUResource::Description UploadBuffer::describeResource() const
        {
            GPUResource::Description description{};
            description.width = mBufferSize;
            description.dimension = GPUResourceDimension::Buffer;

            return description;
        }
    }
}

