#pragma once

#include "common.h"
#include "DescriptorHeapEnums.h"
#include "dx12/DescriptorHeap.h"

namespace graphics
{
    class DescriptorHeapPool
    {
    public:
        DescriptorHeapPool(const size_t numDescriptorsPerHeap = 256);

        inline const size_t getNumDescriptors() const { return mNumDescriptorsPerHeap; }

        dx12::DescriptorHeap& requestDescriptorHeap(const DescriptorHeapType type);
        void discardDescriptorHeap(const uint64_t completedFence, dx12::DescriptorHeap& usedHeaps);
        void destroyPool();

        ~DescriptorHeapPool();

    private:
        // Goes through the used queue and puts all the allocators that have completed their fence into available queue.
        void updateAvailableQueue(const DescriptorHeapType type);

    private:
        const size_t mNumDescriptorsPerHeap;

        // The memory pool containing all the descriptor heaps.
        static std::array<std::vector<dx12::DescriptorHeap>, DescriptorHeapType::DescriptorHeapTypeCount> msDescriptorHeapPool;

        // Once a heap is discarded, it is kept in the used queue.
        static std::array<std::queue<std::pair<uint64_t, dx12::DescriptorHeap*>>, DescriptorHeapType::DescriptorHeapTypeCount> msUsedQueue;

        // Once the fence value of a descriptor inside used queue is reached, ie gpu is done with the heap,
        // it is put inside the available queue ready for reuse.
        static std::array<std::queue<dx12::DescriptorHeap*>, DescriptorHeapType::DescriptorHeapTypeCount> msAvailableQueue;

        // Prevents incorrect reading and writing of descriptor heap pool, used queue and the available queue.
        static std::array<std::mutex, DescriptorHeapType::DescriptorHeapTypeCount> msPoolMutex;
    };
}
