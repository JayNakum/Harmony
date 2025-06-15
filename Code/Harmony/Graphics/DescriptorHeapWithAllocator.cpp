#include "DescriptorHeapWithAllocator.h"

namespace graphics
{
    DescriptorHeapWithAllocator::DescriptorHeapWithAllocator(dx12::Device& device, const Description& createDescription)
        : dx12::DescriptorHeap(device, createDescription), mAllocator((createDescription.maxDescriptors* mDescriptorSize) / PAGE_SIZE) {}

    DescriptorHeapWithAllocator::Page::Page(dx12::DescriptorHandle starthandle, const size_t descriptorSize, const size_t pageSize)
        : mDescriptorSize(descriptorSize), mStartHandle(starthandle), mAllocator(pageSize * mDescriptorSize)
    {
        reset();
    }

    dx12::DescriptorHandle DescriptorHeapWithAllocator::Page::allocate(const size_t count)
    {
        const size_t allocationByteOffset = mAllocator.allocate(count * mDescriptorSize);

        return (mStartHandle + (INT)allocationByteOffset);
    }

    void DescriptorHeapWithAllocator::Page::reset()
    {
        mAllocator.reset();
    }

    DescriptorHeapWithAllocator::Page::~Page()
    {
        reset();
    }

    DescriptorHeapWithAllocator::Page DescriptorHeapWithAllocator::allocatePage()
    {
        std::lock_guard<std::mutex> scopedLockGuard(mHeapMutex);

        const size_t allocationByteOffset = mAllocator.allocate(PAGE_SIZE * mDescriptorSize);

        return Page((mHeapStart + (INT)allocationByteOffset), mDescriptorSize, PAGE_SIZE);
    }

    DescriptorHeapWithAllocator::~DescriptorHeapWithAllocator()
    {
        mAllocator.reset();
    }
}
