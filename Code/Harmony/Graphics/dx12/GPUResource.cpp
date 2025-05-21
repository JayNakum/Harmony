#include "GPUResource.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        void GPUResource::createCommitted(Device& device)
        {
            static Heap::Description description{};
            description.name = "temp description for properties";
            description.sizeInBytes = 0;
            description.type = HeapType::Default;
            static D3D12_HEAP_PROPERTIES heapProperties = Heap::toD3DHeapProperties(description);

            const Description createDescription = describeResource();
            D3D12_RESOURCE_DESC resourceDescription = GPUResource::toD3DResourceDesc(createDescription);

            const D3D12_CLEAR_VALUE clearValue{
                resourceDescription.Format,
                {
                    createDescription.clearColor[0],
                    createDescription.clearColor[1],
                    createDescription.clearColor[2],
                    createDescription.clearColor[3]
                }
            };

            HRESULT hResult = device->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription,
                toD3DResourceStates(mUsageState),
                createDescription.dimension == GPUResourceDimension::Buffer ? nullptr : &clearValue,
                IID_PPV_ARGS(getAddressOf())
            );
            HASSERT(SUCCEEDED(hResult), "Failed to create committed resource %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            setD3DName(mResource.Get(), utility::toWideString(createDescription.name).c_str());
        }

        void GPUResource::createPlaced(Device& device, Heap& backingHeap, uint64_t heapOffset)
        {
            const Description createDescription = describeResource();
            D3D12_RESOURCE_DESC resourceDescription = GPUResource::toD3DResourceDesc(createDescription);

            const D3D12_CLEAR_VALUE clearValue{
                resourceDescription.Format,
                {
                    createDescription.clearColor[0],
                    createDescription.clearColor[1],
                    createDescription.clearColor[2],
                    createDescription.clearColor[3]
                }
            };

            HRESULT hResult = device->CreatePlacedResource(
                (backingHeap.get()), heapOffset, &resourceDescription,
                toD3DResourceStates(mUsageState),
                createDescription.dimension == GPUResourceDimension::Buffer ? nullptr : &clearValue,
                IID_PPV_ARGS(getAddressOf())
            );
            HASSERT(SUCCEEDED(hResult), "Failed to create placed resource %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));
            setD3DName(mResource.Get(), utility::toWideString(createDescription.name).c_str());
        }

        const D3D12_RESOURCE_DESC GPUResource::toD3DResourceDesc(const Description& description)
        {
            D3D12_RESOURCE_DESC createDesc{};
            createDesc.Alignment = description.alignment;
            createDesc.DepthOrArraySize = (UINT16)description.depth;
            createDesc.Dimension = toD3DResourceDimension(description.dimension);
            createDesc.Format = toDXGIFormat(description.format);
            createDesc.Height = (UINT)description.height;
            createDesc.Width = (UINT64)description.width;
            createDesc.Layout = description.dimension == GPUResourceDimension::Buffer ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN;
            createDesc.MipLevels = description.mipLevels;
            createDesc.SampleDesc.Count = description.sampleCount;
            createDesc.SampleDesc.Quality = description.sampleQuality;
            createDesc.Flags = toD3DResourceFlags(description.resourceFlags);
            return createDesc;
        }
    }
}
