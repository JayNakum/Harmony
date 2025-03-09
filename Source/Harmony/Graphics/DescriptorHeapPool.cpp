#include "DescriptorHeapPool.h"

#include "Renderer.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    using namespace dx12;

    DescriptorHeapPool::DescriptorHeapPool(const size_t numDescriptorsPerHeap)
        : mNumDescriptorsPerHeap(numDescriptorsPerHeap)
    {}

    DescriptorHeap& DescriptorHeapPool::requestDescriptorHeap(const DescriptorHeapType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

        updateAvailableQueue(type);

        if (!msAvailableQueue[type].empty())
        {
            DescriptorHeap* descriptorHeap = msAvailableQueue[type].front();
            msAvailableQueue[type].pop();
            return *descriptorHeap;
        }

        DescriptorHeap::Description createDesc;
        createDesc.type = type;
        createDesc.name = (msDescriptorHeapPool[type].size() + " shader visible descriptor heap");
        createDesc.maxDescriptors = mNumDescriptorsPerHeap;

        msDescriptorHeapPool[type].emplace_back(createDesc);

        return msDescriptorHeapPool[type].back();
    }

    void DescriptorHeapPool::discardDescriptorHeap(const uint64_t completedFenceValue, dx12::DescriptorHeap& usedHeaps)
    {
        const DescriptorHeapType type = usedHeaps.getDescriptorHeapType();

        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

        msUsedQueue[type].push(std::make_pair(completedFenceValue, &usedHeaps));
    }

    void DescriptorHeapPool::destroyPool()
    {
        for (size_t type = 0; type < DescriptorHeapType::DescriptorHeapTypeCount; ++type)
        {
            std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

            updateAvailableQueue(DescriptorHeapType(type));
            HASSERT(msUsedQueue[type].empty(), "Trying to destroy descriptor heap pool %d while %d heaps are being used.", type, msUsedQueue[type].size());

            // clear the queues to prevent dangling pointers
            while (!msAvailableQueue[type].empty())
            {
                msAvailableQueue[type].pop();
            }

            msDescriptorHeapPool[type].clear();
        }
    }

    DescriptorHeapPool::~DescriptorHeapPool()
    {
        destroyPool();
    }

    void DescriptorHeapPool::updateAvailableQueue(const DescriptorHeapType type)
    {
        while (!msUsedQueue[type].empty() && GfxCommandListManager().isFenceComplete(msUsedQueue[type].front().first))
        {
            msAvailableQueue[type].push(msUsedQueue[type].front().second);
            msUsedQueue[type].pop();
        }
    }
}
