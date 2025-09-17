#include "GPUResource.h"

namespace graphics
{
    void GPUResource::createCommitted(Device& device, const HeapType heapType)
    {
        const Heap::Description description{
            "temp description for properties",
            0,
            heapType
        };
        const D3D12_HEAP_PROPERTIES heapProperties = Heap::toD3DHeapProperties(description);

        D3D12_RESOURCE_DESC resourceDescription = GPUResource::toD3DResourceDesc(mDescription);

        /*const D3D12_CLEAR_VALUE clearValue{
            resourceDescription.Format,
            {
                mDescription.clearColor[0],
                mDescription.clearColor[1],
                mDescription.clearColor[2],
                mDescription.clearColor[3]
            }
        };*/

        HRESULT hResult = device->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription,
            dx12::toD3DResourceStates(mUsageState),
            nullptr,
            IID_PPV_ARGS(getAddressOf())
        );
        HASSERT(SUCCEEDED(hResult), "Failed to create committed resource %s.\n%s", mDescription.name.c_str(), utility::translateErrorCode(hResult).c_str());

        setD3DName(mResource.Get(), utility::toWideString(mDescription.name).c_str());
    }

    void GPUResource::createPlaced(Device& device, Heap& backingHeap, uint64_t heapOffset)
    {
        mHeapOffset = heapOffset;
        D3D12_RESOURCE_DESC resourceDescription = GPUResource::toD3DResourceDesc(mDescription);

        /*const D3D12_CLEAR_VALUE clearValue{
            resourceDescription.Format,
            {
                mDescription.clearColor[0],
                mDescription.clearColor[1],
                mDescription.clearColor[2],
                mDescription.clearColor[3]
            }
        };*/

        HRESULT hResult = device->CreatePlacedResource(
            (backingHeap.get()), heapOffset, &resourceDescription,
            dx12::toD3DResourceStates(mUsageState),
            nullptr,
            IID_PPV_ARGS(getAddressOf())
        );
        HASSERT(SUCCEEDED(hResult), "Failed to create placed resource %s.\n%s", mDescription.name.c_str(), utility::translateErrorCode(hResult).c_str());
        setD3DName(mResource.Get(), utility::toWideString(mDescription.name).c_str());
    }

    void* GPUResource::map(Device& device)
    {
        void* memory = nullptr;
        CD3DX12_RANGE range = CD3DX12_RANGE(0, mDescription.width);
        HRESULT hResult = mResource->Map(0, &range, &memory);
        HASSERT(SUCCEEDED(hResult), "Failed to map upload buffer resource.\n%s", utility::translateErrorCode(hResult).c_str());
        device.getDeviceRemovedReason();

        return memory;
    }

    void GPUResource::unmap(const size_t begin, size_t end)
    {
        CD3DX12_RANGE range = CD3DX12_RANGE(begin, std::min(end, mDescription.width));
        mResource->Unmap(0, &range);
    }

    const D3D12_RESOURCE_DESC GPUResource::toD3DResourceDesc(const Description& description)
    {
        D3D12_RESOURCE_DESC createDesc{};
        createDesc.Alignment = description.alignment;
        createDesc.DepthOrArraySize = (UINT16)description.depth;
        createDesc.Dimension = dx12::toD3DResourceDimension(description.dimension);
        createDesc.Format = dx12::toDXGIFormat(description.format);
        createDesc.Height = (UINT)description.height;
        createDesc.Width = (UINT64)description.width;
        createDesc.Layout = description.dimension == GPUResourceDimension::Buffer ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN;
        createDesc.MipLevels = description.mipLevels;
        createDesc.SampleDesc.Count = description.sampleCount;
        createDesc.SampleDesc.Quality = description.sampleQuality;
        createDesc.Flags = dx12::toD3DResourceFlags(description.resourceFlags);
        return createDesc;
    }
}
