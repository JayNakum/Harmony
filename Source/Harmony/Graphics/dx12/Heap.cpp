#include "Heap.h"

#include "types/HeapType.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        Heap::Heap(const Description& createDescription)
            : mDescription(createDescription), mNextAllocationOffset(0), mFreeSpace(createDescription.sizeInBytes)
        {
            // TODO: Implement heap creation
        }

        uint64_t Heap::allocate(const uint64_t sizeInBytes)
        {
            HASSERT(canAllocate(sizeInBytes), "%s Heap out of space. Trying to allocate %d bytes but available space is %d bytes.", mDescription.name, sizeInBytes, mFreeSpace);
            uint64_t allocationOffset = mNextAllocationOffset;

            mNextAllocationOffset += sizeInBytes;
            mFreeSpace -= sizeInBytes;

            return allocationOffset;
        }

        void Heap::reset()
        {
            mNextAllocationOffset = 0;
            mFreeSpace = mDescription.sizeInBytes;
        }

        Heap::~Heap()
        {
            mHeap = nullptr;
        }

        D3D12_HEAP_PROPERTIES Heap::toHeapProperties(const Description& description)
        {
            D3D12_HEAP_PROPERTIES heapProperties{};

            heapProperties.Type = heapType.at(description.type);
            heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProperties.CreationNodeMask = 1;
            heapProperties.VisibleNodeMask = 1;

            return heapProperties;
        }
    }
}
