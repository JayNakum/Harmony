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
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        updateAvailableQueue();

        if (!mAvailableQueue.empty())
        {
            dx12::CommandAllocator* allocator = mAvailableQueue.front();
            mAvailableQueue.pop();
            return *allocator;
        }

        dx12::CommandAllocator::Description createDesc;
        createDesc.name = (mCmdAllocatorPool.size() + " " + toString(mType) + "type command allocator");
        createDesc.type = mType;
        mCmdAllocatorPool.emplace_back(createDesc);

        return mCmdAllocatorPool.back();
    }

    void CommandAllocatorPool::discardCommandAllocator(const uint64_t completedFenceValue, dx12::CommandAllocator& usedAllocator)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        mUsedQueue.push(std::make_pair(completedFenceValue, &usedAllocator));
    }

    void CommandAllocatorPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        updateAvailableQueue();
        HASSERT(mUsedQueue.empty(), "Trying to destroy %s type pool while %d allocators are still in used.", toString(mType), mUsedQueue.size());

        // clear the queues to prevent dangling pointers
        while (!mAvailableQueue.empty())
        {
            mAvailableQueue.pop();
        }

        mCmdAllocatorPool.clear();
    }

    CommandAllocatorPool::~CommandAllocatorPool()
    {
        destroyPool();
    }

    void CommandAllocatorPool::updateAvailableQueue()
    {
        while (!mUsedQueue.empty() && GfxCommandListManager().isFenceComplete(mUsedQueue.front().first))
        {
            mAvailableQueue.push(mUsedQueue.front().second);
            mUsedQueue.pop();
        }
    }
}
