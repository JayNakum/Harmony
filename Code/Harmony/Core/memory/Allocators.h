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
        LinearAllocator(const size_t totalSizeInBytes)
            : mTotalSizeInBytes(totalSizeInBytes), mNextAllocationOffset(0) {}

        inline const size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline const size_t getRemainingSize() const
        {
            return mTotalSizeInBytes - mNextAllocationOffset;
        }

        inline const bool canAllocate(const size_t sizeInBytes) const
        {
            return sizeInBytes <= getRemainingSize();
        }

        inline const size_t allocate(const size_t sizeInBytes)
        {
            HASSERT(canAllocate(sizeInBytes), "Allocator out of memory(%d). Trying to allocate %d. Remaining size = %d",
                    mTotalSizeInBytes, sizeInBytes, getRemainingSize());

            size_t allocationOffset = mNextAllocationOffset + sizeInBytes;
            mNextAllocationOffset += sizeInBytes;
            return allocationOffset;
        }

        inline void reset()
        {
            mNextAllocationOffset = 0;
        }

        LinearAllocator(const LinearAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes), mNextAllocationOffset(copy.mNextAllocationOffset) {}

        inline void operator=(const LinearAllocator& rhs)
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
    * Linear Allocator with best fit strategy and coalescing.
    * Linear allocation means that the core allocation logic is simple.
    * Best fit ensures that we reduce fragmentation.
    * Coalescing means that even if there is fragmentation, it will eventually be reused when an adjacent resource is freed.
    *
    * For minimum fragmentation use for same category resources that all have same alignment.
    */
    class SuballocationAllocator
    {
    private:
        struct FreeBlock { size_t offset, size; };

    public:
        SuballocationAllocator(const size_t totalSizeInBytes)
            : mTotalSizeInBytes(totalSizeInBytes), mNextAllocationOffset(0), mFreeList(mTotalSizeInBytes) {}

        inline const size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline const size_t getFragmentedSize() const
        {
            size_t fragmentedSize = 0;
            for (const FreeBlock& block : mFreeList)
            {
                fragmentedSize += block.size;
            }
            return fragmentedSize;
        }

        inline const size_t getUnallocatedSize() const
        {
            return mTotalSizeInBytes - mNextAllocationOffset;
        }

        // Allocate using best fit strategy.
        inline const size_t allocate(const size_t sizeInBytes)
        {
            freeEmptyBlocks();

            // Since freeEmptyBlocks() sorts the list in descending order
            // iterate the list from back using a countdown loop, in order to achieve best fit strategy.
            for (size_t i = mFreeList.size(); i-- > 0;)
            {
                FreeBlock& block = mFreeList[i];

                if (block.size >= sizeInBytes)
                {
                    size_t allocationOffset = block.offset;
                    block.offset += sizeInBytes;
                    block.size -= sizeInBytes;
                    return allocationOffset;
                }
            }

            // New allocation
            HASSERT_SOFT(getFragmentedSize() < sizeInBytes,
                         "There is %d bytes space available but the memory is fragmented. Total size = %d. Fragmented size = %d. Unallocated Size = %d.",
                         sizeInBytes, mTotalSizeInBytes, getFragmentedSize(), getUnallocatedSize());

            HASSERT(getUnallocatedSize() >= sizeInBytes,
                    "Allocator out of memory trying to allocate %d bytes. Total size = %d. Fragmented size = %d. Unallocated Size = %d.",
                    sizeInBytes, mTotalSizeInBytes, getFragmentedSize(), getUnallocatedSize());

            size_t allocationOffset = mNextAllocationOffset + sizeInBytes;
            mNextAllocationOffset += sizeInBytes;
            return allocationOffset;
        }

        // Coalesce and update the freeList.
        inline void deallocate(const size_t allocationOffset, const size_t sizeInBytes)
        {
            HASSERT_SOFT(sizeInBytes > 0, "Trying to free 0 bytes of memory. freeEmptyBlocks() will trim this block.");

            bool didCoalesce = false;
            
            for (size_t i = 0; i < mFreeList.size(); ++i)
            {
                FreeBlock& block = mFreeList[i];

                if (block.offset + block.size == allocationOffset)
                {
                    block.size += sizeInBytes;
                    didCoalesce = true;
                    break;
                }
                
                if (allocationOffset + sizeInBytes == block.offset)
                {
                    block.offset = allocationOffset;
                    block.size += sizeInBytes;
                    didCoalesce = true;
                    break;
                }
            }

            if (!didCoalesce)
            {
                mFreeList.emplace_back(allocationOffset, sizeInBytes);
            }
        }

        inline void reset()
        {
            mFreeList.clear();
            mNextAllocationOffset = 0;
        }

        SuballocationAllocator(const SuballocationAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes), mNextAllocationOffset(copy.mNextAllocationOffset) {}

        inline void operator=(const SuballocationAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalSizeInBytes = rhs.mTotalSizeInBytes;
                mNextAllocationOffset = rhs.mNextAllocationOffset;

                mFreeList.clear();
                mFreeList = rhs.mFreeList;
            }
        }

    private:
        // Sorts the freeList in descending order and removes all the blocks with size 0.
        inline void freeEmptyBlocks()
        {
            // Sort the list in descending order because it is faster to remove the empty blocks from the back.
            std::sort(mFreeList.begin(), mFreeList.end(), [](FreeBlock& a, FreeBlock& b) { return a.size > b.size; });

            size_t newSize = mFreeList.size();
            while (newSize > 0 && mFreeList[newSize - 1].size == 0)
            {
                --newSize;
            }

            mFreeList.resize(newSize);
        }

    private:
        size_t mTotalSizeInBytes;
        size_t mNextAllocationOffset;
        std::vector<FreeBlock> mFreeList;
    };
}

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
        RingAllocator(const size_t totalSizeInBytes)
            : mTotalSizeInBytes(totalSizeInBytes), mNextAllocationOffset(0) {}

        inline const size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline const size_t getRemainingSize() const
        {
            return mTotalSizeInBytes - mNextAllocationOffset;
        }

        inline const bool canAllocate(const size_t sizeInBytes) const
        {
            return sizeInBytes <= getRemainingSize();
        }

        inline const size_t allocate(const size_t sizeInBytes)
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

        RingAllocator(const RingAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes), mNextAllocationOffset(copy.mNextAllocationOffset) {}

        inline void operator=(const RingAllocator& rhs)
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
