#pragma once

#include "common.h"
#include "Log.h"

namespace memory
{
    /*
    * A ring allocator but it ensures that the allocation is contiguous.
    * Meaning that it is possible that there is (some space < sizeInBytes) available at the end
    * but we do not care about it and loop back.
    *
    * Ideal for transient or short lived resources.
    * For minimum wastage use for smaller or aligned resources.
    */
    class RingAllocator
    {
    public:
        RingAllocator(const size_t totalSizeInBytes, const size_t offset = 0)
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
            const bool fitsInTail = canAllocate(sizeInBytes);

            const long long int fitMask = -(long long int)fitsInTail;
            const size_t allocationOffset = (mNextAllocationOffset & fitMask) | (mOffset & ~fitMask);
            mNextAllocationOffset = allocationOffset + sizeInBytes;
            return allocationOffset;
        }

        inline void reset()
        {
            mNextAllocationOffset = mOffset;
        }

        RingAllocator(const RingAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes),
            mOffset(copy.mOffset),
            mNextAllocationOffset(copy.mNextAllocationOffset)
        {}

        inline void operator=(const RingAllocator& rhs)
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
