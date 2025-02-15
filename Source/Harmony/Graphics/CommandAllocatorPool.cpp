#include "CommandAllocatorPool.h"

#include "Renderer.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    using namespace dx12;

    CommandAllocatorPool::CommandAllocatorPool(CommandListType type)
        : CommandObject(type) {}

    CommandAllocator& CommandAllocatorPool::requestCommandAllocator()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        updateAvailableQueue();

        std::queue<CommandAllocator*>& availableQueue = mAvailableQueue;

        if (!availableQueue.empty())
        {
            CommandAllocator* allocator = availableQueue.front();
            availableQueue.pop();
            return *allocator;
        }

        CommandAllocator::Description createDesc;
        createDesc.name = (mCmdAllocatorPool.size() + " type[" + std::to_string(mType) + "] command allocator");
        createDesc.type = mType;
        mCmdAllocatorPool.emplace_back(createDesc);

        return mCmdAllocatorPool.back();
    }

    void CommandAllocatorPool::discardCommandAllocator(const uint64_t completedFenceValue, CommandAllocator& usedAllocator)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        mUsedQueue.push(std::make_pair(completedFenceValue, &usedAllocator));
    }

    void CommandAllocatorPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        updateAvailableQueue();
        HASSERT(mUsedQueue.empty(), "Trying to destroy pool %d while %d allocators are still in used.", mType, mUsedQueue.size());

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
