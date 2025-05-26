#include "DescriptorHeapWithAllocator.h"

#include "Log.h"

namespace graphics
{
    DescriptorHeapWithAllocator::DescriptorHeapWithAllocator(dx12::Device& device, const Description& createDescription)
        : dx12::DescriptorHeap(device, createDescription), mPageOffset(0)
    {
    }

    DescriptorHeapWithAllocator::Page::Page(dx12::DescriptorHandle starthandle, const size_t descriptorSize, const size_t pageSize)
        : mStartHandle(starthandle), mDescriptorSize(descriptorSize), mPageSize(pageSize)
    {
        reset();
    }

    dx12::DescriptorHandle DescriptorHeapWithAllocator::Page::allocate(const size_t count)
    {
        HASSERT_SOFT_ONCE(canAllocate(count), "Descriptor Heap Page out of space. Trying to allocate %d descriptors but available space is %d. Overwriting from the beginning.", count, mNumFreeDescriptors);

        if (!canAllocate(count)) // Ring allocation
        {
            // It is possible that there is (some space < count) available at the end of the heap but we do not care about it.
            // TODO: IF NEEDED implement a better allocation strategy.
            reset();
        }

        dx12::DescriptorHandle allocationHandle = mStartHandle + mNextFreeHandle;

        mNextFreeHandle += (INT)(count * mDescriptorSize);
        mNumFreeDescriptors -= count;

        return allocationHandle;
    }

    void DescriptorHeapWithAllocator::Page::reset()
    {
        mNumFreeDescriptors = mPageSize;
        mNextFreeHandle = 0;
    }

    DescriptorHeapWithAllocator::Page::~Page()
    {
        reset();
    }

    DescriptorHeapWithAllocator::Page DescriptorHeapWithAllocator::allocatePage()
    {
        std::lock_guard<std::mutex> scopedLockGuard(mHeapMutex);

        dx12::DescriptorHandle pageStart = mHeapStart + (mPageOffset * mDescriptorSize);

        mPageOffset += PAGE_SIZE;

        if (mPageOffset > mMaxDescriptors)
        {
            HASSERT_SOFT(false, "Descriptor Heap out of space. Overwriting from the beginning.");
            mPageOffset = 0;
        }

        return Page(pageStart, mDescriptorSize, PAGE_SIZE);
    }

    DescriptorHeapWithAllocator::~DescriptorHeapWithAllocator()
    {
        mPageOffset = 0;
    }
}
