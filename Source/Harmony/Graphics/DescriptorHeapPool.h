#pragma once

#include "common.h"

#include "DescriptorHeapEnums.h"

#include "dx12/DescriptorHeap.h"

namespace graphics
{
    class DescriptorHeapPool
    {
    public:
        struct Description
        {
            DescriptorHeapType type;
            const size_t numDescriptorsPerHeap = 256;
            const DescriptorHeapFlag flag = DescriptorHeapFlag::GPUVisible;
        };
        DescriptorHeapPool(const Description& description);

        inline const size_t getNumDescriptors() const { return mDescription.numDescriptorsPerHeap; }
        inline const DescriptorHeapType getDescriptorType() const { return mDescription.type; }

        dx12::DescriptorHeap& requestDescriptorHeap();
        void discardDescriptorHeap(const uint64_t completedFence, dx12::DescriptorHeap& usedHeaps);
        void destroyPool();

        ~DescriptorHeapPool();

    private:
        // Goes through the used queue and puts all the allocators that have completed their fence into available queue.
        void updateAvailableQueue();

    private:
        const Description mDescription;

        // The memory pool containing all the descriptor heaps.
        std::vector<dx12::DescriptorHeap> mDescriptorHeapPool;

        // Once a heap is discarded, it is kept in the used queue.
        std::queue<std::pair<uint64_t, dx12::DescriptorHeap*>> mUsedQueue;

        // Once the fence value of a descriptor inside used queue is reached, ie gpu is done with the heap,
        // it is put inside the available queue ready for reuse.
        std::queue<dx12::DescriptorHeap*> mAvailableQueue;

        // Prevents incorrect reading and writing of descriptor heap pool, used queue and the available queue.
        std::mutex mPoolMutex;
    };
}
