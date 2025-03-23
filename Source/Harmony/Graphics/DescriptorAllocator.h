#pragma once

#include "common.h"

#include "DescriptorHeapEnums.h"

#include "dx12/DescriptorHeap.h"

namespace graphics
{
    class CommandContext;
}

namespace graphics
{
    class DescriptorHeapPool
    {
    public:
        struct Description
        {
            DescriptorHeapType type;
            const size_t numDescriptorsPerHeap = 256;
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

namespace graphics
{
    /*
    * An engineered class that mamages the following things:
    * - separate CPU and GPU visible heaps
    * - staging multiple descriptors from CPU to GPU visible heaps
    * - dynamic sized descriptor heaps
    *
    * allocateDescriptors() always returns a handle from CPU visible heap.
    * Only when the descriptor is staged, it is scheduled for a copy to the GPU visible heap.
    * finalize() does the copy.
    */
    class DescriptorAllocator
    {
    public:
        DescriptorAllocator(const DescriptorHeapType type, const size_t initialGPUDescriptors = 16);

        dx12::DescriptorHandle allocate(const size_t count = 1);

        // Adds the descriptorHandle to the StagingList
        void stageDescriptors(dx12::DescriptorHandle& handle, const size_t count = 1);

        // Returns the handle from GPU Descriptor heap.
        // Copies the handles in StagingList to GPU visible descriptor heap.
        dx12::DescriptorHandle finalize();

        ~DescriptorAllocator();

    private:
        const DescriptorHeapType mType;

        struct Descriptor
        {
            dx12::DescriptorHandle handle;
            size_t count = 1;
            bool isDirty = false;
        };
        std::vector<Descriptor> mStagingCache;

        dx12::DescriptorHeap* mCPUDescriptorHeap;
        DescriptorHeapPool mCPUDescriptorHeapPool;

        std::mutex mHeapMutex;
        // The GPU Visible Heap with maxDescriptors = initialGPUDescriptors
        // TODO: make the size dynamically change (similar to std::vector)
        dx12::DescriptorHeap mGPUDescriptorHeap;
    };
}
