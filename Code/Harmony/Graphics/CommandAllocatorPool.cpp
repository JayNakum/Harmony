#include "CommandAllocatorPool.h"

#include "Log.h"

namespace graphics
{
    CommandAllocatorPool::CommandAllocatorPool(const CommandListType type)
        : CommandObject(type)
    {}

    dx12::CommandAllocator& CommandAllocatorPool::requestCommandAllocator(dx12::Device& device, const uint64_t completedFenceValue)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mPoolMutex);

        updateAvailableQueue(completedFenceValue);

        HASSERT(mCmdAllocatorPool.size() <= MAX_COMMAND_ALLOCATORS, "Command Allocator Pool Budget exceeded.");

        if (!mAvailableQueue.empty())
        {
            dx12::CommandAllocator* allocator = mAvailableQueue.front();
            mAvailableQueue.pop();
            (*allocator)->Reset();
            return *allocator;
        }

        dx12::CommandAllocator::Description createDesc;
        createDesc.name = (mCmdAllocatorPool.size() + " " + toString(mType) + " type command allocator");
        createDesc.type = mType;
        mCmdAllocatorPool.emplace_back(device, createDesc);

        return mCmdAllocatorPool.back();
    }

    void CommandAllocatorPool::discardCommandAllocator(const uint64_t completedFenceValue, dx12::CommandAllocator& usedAllocator)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mPoolMutex);

        mUsedQueue.push(std::make_pair(completedFenceValue, &usedAllocator));
    }

    void CommandAllocatorPool::destroyPool()
    {
        std::lock_guard<std::mutex> scopedLockGuard(mPoolMutex);

        // clear the queues to prevent dangling pointers
        while (!mUsedQueue.empty())
        {
            mUsedQueue.pop();
        }
        while (!mAvailableQueue.empty())
        {
            mAvailableQueue.pop();
        }

        for (dx12::CommandAllocator& allocator : mCmdAllocatorPool)
        {
            allocator->Reset();
        }

        mCmdAllocatorPool.clear();
    }

    CommandAllocatorPool::~CommandAllocatorPool()
    {
        destroyPool();
    }

    void CommandAllocatorPool::updateAvailableQueue(const uint64_t completedFenceValue)
    {
        while (!mUsedQueue.empty() && (mUsedQueue.front().first) < completedFenceValue)
        {
            mAvailableQueue.push(mUsedQueue.front().second);
            mUsedQueue.pop();
        }
    }
}
