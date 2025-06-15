#pragma once

#include "common.h"
#include "Log.h"
#include "memory/Allocators.h"

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

            inline const memory::ContiguousRingAllocator getAllocator() const { return mAllocator; }
            inline dx12::DescriptorHandle getStartHandle() const { return mStartHandle; }
            inline dx12::DescriptorHandle getStartHandle() { return mStartHandle; }

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

            // Creates space for count number of descriptors inside the heap. Using ring allocation.
            // Returns the handle that points to the start of the allocation.
            // The handle will then be used for actual allocation.
            dx12::DescriptorHandle allocate(const size_t count = 1);

            // Sets the allocation handle at start and numFreeDescriptors to maxDescriptors.
            // It doesn't invalidate descriptors.
            void reset();

            Page(const Page& copy)
                : mDescriptorSize(copy.mDescriptorSize), mStartHandle(copy.mStartHandle), mAllocator(copy.mAllocator) {}

            void operator=(const Page& rhs)
            {
                HASSERT(mAllocator.getTotalSize() == rhs.mAllocator.getTotalSize(), "Invalid copy operation");
                if (this != &rhs)
                {
                    mStartHandle = rhs.mStartHandle;
                    mAllocator = rhs.mAllocator;
                }
            }

            ~Page();

        private:
            const size_t mDescriptorSize;
            dx12::DescriptorHandle mStartHandle;
            memory::ContiguousRingAllocator mAllocator;
        };

        Page allocatePage();

        inline const memory::ContiguousRingAllocator getAllocator() const { return mAllocator; }

        // Resets the allocation pointers.
        ~DescriptorHeapWithAllocator();

    private:
        memory::ContiguousRingAllocator mAllocator;
        std::mutex mHeapMutex;
    };
}
