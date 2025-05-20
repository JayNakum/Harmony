#pragma once

#include "common.h"

#include "dx12/DescriptorHeap.h"

namespace graphics
{
    /*
    * DescriptorHeap with a ring allocator.
    * It starts overwriting descriptors from beginning once the heap is full.
    */
    class DescriptorHeapWithAllocator : public dx12::DescriptorHeap
    {
        static constexpr size_t PAGE_SIZE = 256;

    public:
        // Creates a DescriptorHeap and resets the allocation pointers.
        DescriptorHeapWithAllocator(dx12::Device& device, const Description& createDescription);

        class Page
        {
        public:
            Page(dx12::DescriptorHandle starthandle, const size_t descriptorSize, const size_t pageSize);

            inline dx12::DescriptorHandle getStartHandle() const { return mStartHandle; }
            inline dx12::DescriptorHandle getStartHandle() { return mStartHandle; }

            inline const size_t getFreeHandlesCount() const { return mNumFreeDescriptors; }
            inline const size_t getUsedHandlesCount() const { return mPageSize - mNumFreeDescriptors; }

            // Calculates the page offset of the handle.
            inline const uint32_t getOffsetOfHandle(const dx12::DescriptorHandle& dHandle) const
            {
                return (uint32_t)(getByteOffsetOfHandle(dHandle) / mDescriptorSize);
            }

            // Calculates the page offset of the handle.
            inline const uint32_t getByteOffsetOfHandle(const dx12::DescriptorHandle& dHandle) const
            {
                return (uint32_t)((dHandle.getCPUPtr() - mStartHandle.getCPUPtr()));
            }

            // Checks for available space inside page.
            inline bool canAllocate(const size_t count) const { return count <= mNumFreeDescriptors; }

            // Operator overload for array like indexing.
            inline dx12::DescriptorHandle operator[] (const size_t index) const { return mStartHandle + (INT)index * (INT)mDescriptorSize; }

            inline const size_t getDescriptorSize() const { return mDescriptorSize; }

            // Creates space for count number of descriptors inside the heap. Using ring allocation.
            // Returns the handle that points to the start of the allocation.
            // The handle will then be used for actual allocation.
            dx12::DescriptorHandle allocate(const size_t count = 1);

            // Sets the allocation handle at start and numFreeDescriptors to maxDescriptors.
            // It doesn't invalidate descriptors.
            void reset();

            Page(const Page& copy)
                : mStartHandle(copy.mStartHandle),
                mNumFreeDescriptors(copy.mNumFreeDescriptors),
                mNextFreeHandle(copy.mNextFreeHandle),
                mDescriptorSize(copy.mDescriptorSize),
                mPageSize(copy.mPageSize)
            {}

            void operator=(const Page& rhs)
            {
                if (this != &rhs)
                {
                    mStartHandle = rhs.mStartHandle;
                    mNumFreeDescriptors = rhs.mNumFreeDescriptors;
                    mNextFreeHandle = rhs.mNextFreeHandle;
                }
            }

            ~Page();

        private:
            dx12::DescriptorHandle mStartHandle;

            // Keep track of the free spaces available for validation.
            size_t mNumFreeDescriptors;

            // Handle where the next allocation will be done.
            unsigned int mNextFreeHandle;

            const size_t mDescriptorSize;
            const size_t mPageSize;
        };

        Page allocatePage();

        // Resets the allocation pointers.
        ~DescriptorHeapWithAllocator();

    private:
        unsigned int mPageOffset;
        std::mutex mHeapMutex;
    };
}
