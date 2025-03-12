#include "CommandAllocatorPool.h"

#include "Renderer.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    CommandAllocatorPool::CommandAllocatorPool(const CommandListType type)
        : CommandObject(type)
    {}

    dx12::CommandAllocator& CommandAllocatorPool::requestCommandAllocator()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex);

        updateAvailableQueue();

        if (!msAvailableQueue.empty())
        {
            dx12::CommandAllocator* allocator = msAvailableQueue.front();
            msAvailableQueue.pop();
            return *allocator;
        }

        dx12::CommandAllocator::Description createDesc;
        createDesc.name = (msCmdAllocatorPool.size() + " " + toString(mType) + "type command allocator");
        createDesc.type = mType;
        msCmdAllocatorPool.emplace_back(createDesc);

        return msCmdAllocatorPool.back();
    }

    void CommandAllocatorPool::discardCommandAllocator(const uint64_t completedFenceValue, dx12::CommandAllocator& usedAllocator)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex);

        msUsedQueue.push(std::make_pair(completedFenceValue, &usedAllocator));
    }

    void CommandAllocatorPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex);

        updateAvailableQueue();
        HASSERT(msUsedQueue.empty(), "Trying to destroy %s type pool while %d allocators are still in used.", toString(mType), msUsedQueue.size());

        // clear the queues to prevent dangling pointers
        while (!msAvailableQueue.empty())
        {
            msAvailableQueue.pop();
        }

        msCmdAllocatorPool.clear();
    }

    CommandAllocatorPool::~CommandAllocatorPool()
    {
        destroyPool();
    }

    void CommandAllocatorPool::updateAvailableQueue()
    {
        while (!msUsedQueue.empty() && GfxCommandListManager().isFenceComplete(msUsedQueue.front().first))
        {
            msAvailableQueue.push(msUsedQueue.front().second);
            msUsedQueue.pop();
        }
    }
}
