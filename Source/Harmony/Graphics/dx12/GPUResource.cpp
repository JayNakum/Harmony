#include "GPUResource.h"

#include "Renderer.h"

#include "Heap.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        void GPUResource::createCommitted()
        {
            static Heap::Description description{};
            description.name = "temp description for properties";
            description.sizeInBytes = 0;
            description.type = HeapType::Default;
            static D3D12_HEAP_PROPERTIES heapProperties = Heap::toD3DHeapProperties(description);

            const Description createDescription = describeResource();
            D3D12_RESOURCE_DESC resourceDescription = GPUResource::toD3DResourceDesc(createDescription);

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
        }

        void GPUResource::createPlaced(Heap& backingHeap, uint64_t heapOffset)
        {
            const Description createDescription = describeResource();
            D3D12_RESOURCE_DESC resourceDescription = GPUResource::toD3DResourceDesc(createDescription);

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
        }

        const D3D12_RESOURCE_DESC GPUResource::toD3DResourceDesc(const Description& description)
        {
            D3D12_RESOURCE_DESC createDesc{};
            createDesc.Alignment = description.alignment;
            createDesc.DepthOrArraySize = description.depth;
            createDesc.Dimension = toD3DResourceDimension(description.dimension);
            createDesc.Format = toDXGIFormat(description.format);
            createDesc.Height = (UINT)description.height;
            createDesc.Width = (UINT64)description.width;
            createDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            createDesc.MipLevels = description.mipLevels;
            createDesc.SampleDesc.Count = description.sampleCount;
            createDesc.SampleDesc.Quality = description.sampleQuality;
            createDesc.Flags = toD3DResourceFlags(description.resourceFlags);
            return createDesc;
        }
    }
}
