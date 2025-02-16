#include "Heap.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        Heap::Heap(const Description& createDescription)
            : mDescription(createDescription), mNextAllocationOffset(0), mFreeSpace(createDescription.sizeInBytes)
        {}

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
    }
}
