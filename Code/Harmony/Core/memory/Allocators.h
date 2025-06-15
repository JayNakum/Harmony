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
        ContiguousLinearAllocator(const size_t totalSizeInBytes)
            : mNextAllocationOffset(0), mTotalSizeInBytes(totalSizeInBytes)
        {}

        inline const size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline const size_t getRemainingSize() const
        {
            return mTotalSizeInBytes - mNextAllocationOffset;
        }

        inline const bool canAllocate(const size_t sizeInBytes = 1) const
        {
            return sizeInBytes <= getRemainingSize();
        }

        inline const size_t allocate(const size_t sizeInBytes = 1)
        {
            HASSERT(canAllocate(sizeInBytes), "Contiguous Allocator out of memory(%d). Trying to allocate %d. Remaining size = %d",
                    mTotalSizeInBytes, sizeInBytes, getRemainingSize());

            size_t allocationOffset = mNextAllocationOffset + sizeInBytes;

            mNextAllocationOffset += sizeInBytes;

            return allocationOffset;
        }

        inline void reset()
        {
            mNextAllocationOffset = 0;
        }

        ContiguousLinearAllocator(const ContiguousLinearAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes), mNextAllocationOffset(copy.mNextAllocationOffset)
        {}

        inline void operator=(const ContiguousLinearAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalSizeInBytes = rhs.mTotalSizeInBytes;
                mNextAllocationOffset = rhs.mNextAllocationOffset;
            }
        }

    private:
        size_t mTotalSizeInBytes;
        size_t mNextAllocationOffset;
    };
}

namespace memory
{
    /*
    * A ring allocator but it ensures that the allocation is contiguous.
    * Meaning that it is possible that there is (some space < sizeInBytes) available at the end
    * but we do not care about it and loop back.
    *
    * Useful for smaller structs, always aligned structs and cache friendly memory structures.
    */
    class ContiguousRingAllocator
    {
    public:
        ContiguousRingAllocator(const size_t totalSizeInBytes)
            : mTotalSizeInBytes(totalSizeInBytes), mNextAllocationOffset(0) {}

        inline const size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline const size_t getRemainingSize() const
        {
            return mTotalSizeInBytes - mNextAllocationOffset;
        }

        inline const bool canAllocate(const size_t sizeInBytes = 1) const
        {
            return sizeInBytes <= getRemainingSize();
        }

        inline const size_t allocate(const size_t sizeInBytes = 1)
        {
            bool fitsInTail = canAllocate(sizeInBytes);
            size_t allocationOffset = mNextAllocationOffset * fitsInTail;
            mNextAllocationOffset = (allocationOffset + sizeInBytes) % mTotalSizeInBytes;
            return allocationOffset;
        }

        inline void reset()
        {
            mNextAllocationOffset = 0;
        }

        ContiguousRingAllocator(const ContiguousRingAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes), mNextAllocationOffset(copy.mNextAllocationOffset) {}

        inline void operator=(const ContiguousRingAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalSizeInBytes = rhs.mTotalSizeInBytes;
                mNextAllocationOffset = rhs.mNextAllocationOffset;
            }
        }

    private:
        size_t mTotalSizeInBytes;
        size_t mNextAllocationOffset;
    };
}

namespace memory
{
    class BuddyAllocator
    {
    public:
        BuddyAllocator(const size_t totalSizeInBytesInBytes);

    };
}
