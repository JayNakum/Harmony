#include "DescriptorHeapPool.h"

#include "Renderer.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    using namespace dx12;

    DescriptorHeapPool::DescriptorHeapPool(const Description& createDescription)
        : mDescription(createDescription)
    {}

    DescriptorHeap& DescriptorHeapPool::requestDescriptorHeap()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        updateAvailableQueue();

        if (!mAvailableQueue.empty())
        {
            DescriptorHeap* descriptorHeap = mAvailableQueue.front();
            mAvailableQueue.pop();
            return *descriptorHeap;
        }

        DescriptorHeap::Description createDesc;
        createDesc.type = mDescription.type;
        createDesc.name = (mDescriptorHeapPool.size() + " shader visible descriptor heap");
        createDesc.maxDescriptors = mDescription.numDescriptorsPerHeap;

        mDescriptorHeapPool.emplace_back(createDesc);

        return mDescriptorHeapPool.back();
    }

    void DescriptorHeapPool::discardDescriptorHeap(const uint64_t completedFenceValue, dx12::DescriptorHeap& usedHeaps)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        mUsedQueue.push(std::make_pair(completedFenceValue, &usedHeaps));
    }

    void DescriptorHeapPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        updateAvailableQueue();
        HASSERT(mUsedQueue.empty(), "Trying to destroy descriptor heap pool %d while %d heaps are being used.", mDescription.type, mUsedQueue.size());

        // clear the queues to prevent dangling pointers
        while (!mAvailableQueue.empty())
        {
            mAvailableQueue.pop();
        }

        mDescriptorHeapPool.clear();
    }

    void DescriptorHeapPool::updateAvailableQueue()
    {
        while (!mUsedQueue.empty() && GfxCommandListManager().isFenceComplete(mUsedQueue.front().first))
        {
            mAvailableQueue.push(mUsedQueue.front().second);
            mUsedQueue.pop();
        }
    }
}
