#include "UploadBuffer.h"

#include "Renderer.h"

#include "GPUResourceStates.h"

#include "Utility.h"
#include "Log.h"

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
            D3D12_HEAP_PROPERTIES heapProps{};
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            // Upload buffers must be 1-dimensional
            D3D12_RESOURCE_DESC createDesc {};
            createDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            createDesc.Width = mBufferSize;
            createDesc.Height = 1;
            createDesc.DepthOrArraySize = 1;
            createDesc.MipLevels = 1;
            createDesc.Format = DXGI_FORMAT_UNKNOWN;
            createDesc.SampleDesc.Count = 1;
            createDesc.SampleDesc.Quality = 0;
            createDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            createDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

            mUsageState = GPUResourceState::GenericRead;

            HRESULT hResult = GfxDevice()->CreateCommittedResource(
                &heapProps, D3D12_HEAP_FLAG_NONE, &createDesc,
                gpuResourceStates.at(mUsageState), nullptr, IID_PPV_ARGS(&mResource));

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
    }
}

