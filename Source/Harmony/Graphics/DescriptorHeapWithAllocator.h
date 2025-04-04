#pragma once

#include "dx12/DescriptorHeap.h"

namespace graphics
{
    /*
    * DescriptorHeap with a ring allocator.
    * It starts overwriting descriptors from beginning once the heap is full.
    */
    class DescriptorHeapWithAllocator : public dx12::DescriptorHeap
    {
    public:
        // Creates a DescriptorHeap and resets the allocation pointers.
        DescriptorHeapWithAllocator(const Description& createDescription);

        // Checks for available space inside heap.
        inline bool canAllocate(const size_t count) const { return count <= mNumFreeDescriptors; }

        // Creates space for count number of descriptors inside the heap. Using ring allocation.
        // Returns the handle that points to the start of the allocation.
        // The handle will then be used for actual allocation.
        dx12::DescriptorHandle allocate(const size_t count = 1);

        // Sets the allocation handle at start and numFreeDescriptors to maxDescriptors.
        // It doesn't invalidate descriptors.
        void reset();

        // Resets the allocation pointers.
        ~DescriptorHeapWithAllocator();

    private:
        // Keep track of the free spaces available for validation.
        size_t mNumFreeDescriptors;

        // Handle where the next allocation will be done.
        dx12::DescriptorHandle mNextFreeHandle;
    };
}
