#include "Heap.h"

#include "types/HeapType.h"

namespace graphics
{
    namespace dx12
    {
        Heap::Heap(Device& device, const Description& createDescription)
            : mHeap(nullptr)
        {
            D3D12_HEAP_DESC heapDesc{};
            heapDesc.SizeInBytes = (UINT64)createDescription.sizeInBytes;
            heapDesc.Properties.Type = toD3DHeapType(createDescription.type);
            heapDesc.Alignment = PLACEMENT_ALIGNMENT;
            heapDesc.Flags = D3D12_HEAP_FLAG_NONE;
            HRESULT hResult = device->CreateHeap(&heapDesc, IID_PPV_ARGS(&mHeap));
            HASSERT(SUCCEEDED(hResult), "Failed to create heap for opaque render items.\n%s", utility::translateErrorCode(hResult));

            setD3DName(mHeap.Get(), utility::toWideString(createDescription.name).c_str());
        }

        Heap::~Heap()
        {
            mHeap = nullptr;
        }

        D3D12_HEAP_PROPERTIES Heap::toD3DHeapProperties(const Description& description)
        {
            D3D12_HEAP_PROPERTIES heapProperties{};

            heapProperties.Type = toD3DHeapType(description.type);
            heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProperties.CreationNodeMask = 1;
            heapProperties.VisibleNodeMask = 1;

            return heapProperties;
        }
    }
}
