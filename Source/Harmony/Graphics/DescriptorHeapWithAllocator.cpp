#include "DescriptorHeapWithAllocator.h"

#include "Renderer.h"

namespace graphics
{
    DescriptorHeapWithAllocator::DescriptorHeapWithAllocator(const Description& createDescription)
        : dx12::DescriptorHeap(createDescription)
    {
        reset();
    }

    dx12::DescriptorHandle DescriptorHeapWithAllocator::allocate(const size_t count)
    {
        HASSERT_SOFT(canAllocate(count), "%s Descriptor Heap out of space. Trying to allocate %d descriptors but available space is %d. Overwriting from the beginning.", mDescription.name, count, mNumFreeDescriptors);

        if (!canAllocate(count)) // Ring allocation
        {
            // It is possible that there is (some space < count) available at the end of the heap but we do not care about it.
            // TODO: IF NEEDED implement a better allocation strategy.
            reset();
        }

        dx12::DescriptorHandle allocationHandle = mNextFreeHandle;

        mNextFreeHandle += (INT)(count * mDescriptorSize);
        mNumFreeDescriptors -= count;

        return allocationHandle;
    }

    void DescriptorHeapWithAllocator::reset()
    {
        mNumFreeDescriptors = mDescription.maxDescriptors;
        mNextFreeHandle = mHeapStart;
    }

    DescriptorHeapWithAllocator::~DescriptorHeapWithAllocator()
    {
        reset();
    }
}
