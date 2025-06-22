#pragma once

#include "common.h"
#include "Log.h"

namespace memory
{
    /*
    * A basic contiguous linear allocator, also known as a bump allocator or stack allocator, is a
    * simple memory allocation strategy that allocates memory linearly, typically by incrementing a pointer.
    *
    * Ideal for situations where allocations are either short lived or can be deallocated all at once.
    */
    class LinearAllocator
    {
    public:
        LinearAllocator(const size_t totalSizeInBytes, const size_t offset = 0)
            : mTotalSizeInBytes(totalSizeInBytes),
            mOffset(offset),
            mNextAllocationOffset(mOffset)
        {}

        inline const size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline const size_t getRemainingSize() const
        {
            return (mOffset + mTotalSizeInBytes) - mNextAllocationOffset;
        }

        inline const bool canAllocate(const size_t sizeInBytes) const
        {
            return sizeInBytes <= getRemainingSize();
        }

        inline const size_t allocate(const size_t sizeInBytes)
        {
            HASSERT(canAllocate(sizeInBytes), "Allocator out of memory(%d). Trying to allocate %d. Remaining size = %d",
                    mTotalSizeInBytes, sizeInBytes, getRemainingSize());

            size_t allocationOffset = mNextAllocationOffset;
            mNextAllocationOffset += sizeInBytes;
            return allocationOffset;
        }

        inline void reset()
        {
            mNextAllocationOffset = mOffset;
        }

        LinearAllocator(const LinearAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes),
            mOffset(copy.mOffset),
            mNextAllocationOffset(copy.mNextAllocationOffset)
        {}

        inline void operator=(const LinearAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalSizeInBytes = rhs.mTotalSizeInBytes;
                mOffset = rhs.mOffset;
                mNextAllocationOffset = rhs.mNextAllocationOffset;
            }
        }

    private:
        size_t mTotalSizeInBytes;
        size_t mOffset;
        size_t mNextAllocationOffset;
    };
}
