#include "CommandAllocatorPool.h"

namespace graphics
{
    CommandAllocatorPool::CommandAllocatorPool(const CommandListType type)
        : CommandObject(type)
    {
    }

    CommandAllocator& CommandAllocatorPool::requestCommandAllocator(Device& device, const uint64_t completedFenceValue)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mPoolMutex);

        updateAvailableQueue(completedFenceValue);

        HASSERT(mCmdAllocatorPool.size() <= MAX_COMMAND_ALLOCATORS, "Command Allocator Pool Budget exceeded.");

        if (!mAvailableQueue.empty())
        {
            CommandAllocator* allocator = mAvailableQueue.front();
            mAvailableQueue.pop();
            (*allocator)->Reset();
            return *allocator;
        }

        CommandAllocator::Description createDesc;
        createDesc.name = (std::to_string(mCmdAllocatorPool.size()) + " " + toString(mType) + " type command allocator");
        createDesc.type = mType;
        mCmdAllocatorPool.emplace_back(device, createDesc);

        return mCmdAllocatorPool.back();
    }

    void CommandAllocatorPool::discardCommandAllocator(const uint64_t completedFenceValue, CommandAllocator& usedAllocator)
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

        for (CommandAllocator& allocator : mCmdAllocatorPool)
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
