#pragma once

#include "common.h"
#include "Log.h"

namespace memory
{
    /*
    * Simple contiguous linear allocator.
    */
    class ContiguousLinearAllocator
    {
    public:
        ContiguousLinearAllocator(const size_t totalCount)
            : mNextAllocationOffset(0), mTotalCount(totalCount) {}

        inline const size_t getTotalSize() const { return mTotalCount; }

        inline const size_t getRemainingSize() const
        {
            return mTotalCount - mNextAllocationOffset;
        }

        inline const bool canAllocate(const size_t count = 1) const
        {
            return count <= getRemainingSize();
        }

        inline const size_t allocate(const size_t count = 1)
        {
            HASSERT(canAllocate(count), "Contiguous Allocator out of memory(%d). Trying to allocate %d. Remaining size = %d",
                    mTotalCount, count, getRemainingSize());

            size_t allocationOffset = mNextAllocationOffset + count;

            mNextAllocationOffset += count;

            return allocationOffset;
        }

        inline void reset()
        {
            mNextAllocationOffset = 0;
        }

        ContiguousLinearAllocator(const ContiguousLinearAllocator& copy)
            : mTotalCount(copy.mTotalCount), mNextAllocationOffset(copy.mNextAllocationOffset)
        {}

        void operator=(const ContiguousLinearAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalCount = rhs.mTotalCount;
                mNextAllocationOffset = rhs.mNextAllocationOffset;
            }
        }

    private:
        size_t mTotalCount;
        size_t mNextAllocationOffset;
    };


    /*
    * A ring allocator but it ensures that the allocation is contiguous.
    * Meaning that it is possible that there is (some space < count) available at the end
    * but we do not care about it and loop back.
    *
    * Useful for smaller structs, always aligned structs and cache friendly memory structures.
    */
    class ContiguousRingAllocator
    {
    public:
        ContiguousRingAllocator(const size_t totalCount)
            : mTotalCount(totalCount), mNextAllocationOffset(0) {}

        inline const size_t getTotalSize() const { return mTotalCount; }

        inline const size_t getRemainingSize() const
        {
            return mTotalCount - mNextAllocationOffset;
        }

        inline const bool canAllocate(const size_t count = 1) const
        {
            return count <= getRemainingSize();
        }

        inline const size_t allocate(const size_t count = 1)
        {
            bool fitsInTail = canAllocate(count);
            size_t allocationOffset = mNextAllocationOffset * fitsInTail;
            mNextAllocationOffset = (allocationOffset + count) % mTotalCount;
            return allocationOffset;
        }

        inline void reset()
        {
            mNextAllocationOffset = 0;
        }

        ContiguousRingAllocator(const ContiguousRingAllocator& copy)
            : mTotalCount(copy.mTotalCount), mNextAllocationOffset(copy.mNextAllocationOffset)
        {}

        void operator=(const ContiguousRingAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalCount = rhs.mTotalCount;
                mNextAllocationOffset = rhs.mNextAllocationOffset;
            }
        }

    private:
        size_t mTotalCount;
        size_t mNextAllocationOffset;
    };
}
