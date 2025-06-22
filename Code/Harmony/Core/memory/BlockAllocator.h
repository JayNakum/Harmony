#pragma once

#include "common.h"
#include "Log.h"

namespace memory
{
    /*
    * Block Allocator with first fit strategy and coalescing.
    * Allocates in blocks of specified sizes.
    *
    * Ideal for strictly aligned resources.
    */
    class BlockAllocator
    {
        static constexpr unsigned int BLOCK_SIZE = 128;

    private:
        struct FreeBlock { size_t offset, count; };

    public:
        BlockAllocator(const size_t totalSizeInBytes, const size_t blockSize = BLOCK_SIZE, const size_t offset = 0)
            : mTotalSizeInBytes(((totalSizeInBytes + blockSize - 1) / blockSize)* blockSize),
            mBlockSize(blockSize),
            mOffset(offset),
            mNextAllocationOffset(mOffset)
        {
            HASSERT_SOFT((totalSizeInBytes % blockSize) == 0, "Invalid total size %d and block size %d. Aligning up the total size to match block size. New Size = %d.",
                         totalSizeInBytes, blockSize, mTotalSizeInBytes);
        }

        inline const size_t getTotalSize() const { return mTotalSizeInBytes; }

        inline const size_t getFragmentedSize() const
        {
            size_t fragmentedSize = 0;
            for (const FreeBlock& block : mFreeList)
            {
                fragmentedSize += block.count;
            }
            return fragmentedSize * mBlockSize;
        }

        inline const size_t getUnallocatedSize() const
        {
            return (mOffset + mTotalSizeInBytes) - mNextAllocationOffset;
        }

        // Allocate using first fit strategy.
        inline const size_t allocate(const size_t sizeInBytes)
        {
            HASSERT_SOFT((sizeInBytes % mBlockSize) == 0, "Invalid allocation size %d. Aligning up the size to match block size.", sizeInBytes);

            size_t allocationOffset = 0x1deadb0b;

            const size_t blocksToAllocate = (sizeInBytes + mBlockSize - 1) / mBlockSize;

            for (size_t i = 0; i < mFreeList.size(); ++i)
            {
                FreeBlock& block = mFreeList[i];

                if (block.count == blocksToAllocate)
                {
                    allocationOffset = block.offset;
                    mFreeList.erase(mFreeList.begin() + i);
                    return allocationOffset;
                }
                else if (block.count > blocksToAllocate)
                {
                    allocationOffset = block.offset;
                    block.offset += blocksToAllocate * mBlockSize;
                    block.count -= blocksToAllocate;
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

            allocationOffset = mNextAllocationOffset;
            mNextAllocationOffset += (blocksToAllocate * mBlockSize);
            return allocationOffset;
        }

        // Deallocate and coalesce immediately.
        inline void deallocate(const size_t allocationOffset, const size_t sizeInBytes)
        {
            const size_t blocksToFree = (sizeInBytes + mBlockSize - 1) / mBlockSize;

            size_t insertIndex = 0;
            for (; insertIndex < mFreeList.size(); ++insertIndex)
            {
                if (mFreeList[insertIndex].offset > allocationOffset)
                {
                    break;
                }
            }

            bool didCoalesce = false;
            if (insertIndex > 0)
            {
                FreeBlock& previousBlock = mFreeList[insertIndex - 1];
                if (previousBlock.offset + previousBlock.count * mBlockSize == allocationOffset)
                {
                    previousBlock.count += blocksToFree;

                    if (insertIndex < mFreeList.size())
                    {
                        FreeBlock& nextBlock = mFreeList[insertIndex];
                        if (allocationOffset + blocksToFree * mBlockSize == nextBlock.offset)
                        {
                            previousBlock.count += nextBlock.count;
                            mFreeList.erase(mFreeList.begin() + insertIndex);
                        }
                    }

                    didCoalesce = true;
                }
            }

            if (!didCoalesce && insertIndex < mFreeList.size())
            {
                FreeBlock& nextBlock = mFreeList[insertIndex];
                if (allocationOffset + blocksToFree * mBlockSize == nextBlock.offset)
                {
                    nextBlock.offset = allocationOffset;
                    nextBlock.count += blocksToFree;
                    didCoalesce = true;
                }
            }

            if (!didCoalesce)
            {
                mFreeList.insert(mFreeList.begin() + insertIndex, { allocationOffset, blocksToFree });
            }
        }

        inline void reset()
        {
            mFreeList.clear();
            mNextAllocationOffset = mOffset;
        }

        BlockAllocator(const BlockAllocator& copy)
            : mTotalSizeInBytes(copy.mTotalSizeInBytes),
            mBlockSize(copy.mBlockSize),
            mOffset(copy.mOffset),
            mNextAllocationOffset(copy.mNextAllocationOffset)
        {}

        inline void operator=(const BlockAllocator& rhs)
        {
            if (this != &rhs)
            {
                mTotalSizeInBytes = rhs.mTotalSizeInBytes;
                mBlockSize = rhs.mBlockSize;
                mOffset = rhs.mOffset;
                mNextAllocationOffset = rhs.mNextAllocationOffset;

                mFreeList.clear();
                mFreeList = rhs.mFreeList;
            }
        }
    private:
        size_t mTotalSizeInBytes;
        size_t mBlockSize;
        size_t mOffset;
        size_t mNextAllocationOffset;
        std::vector<FreeBlock> mFreeList;
    };
}
