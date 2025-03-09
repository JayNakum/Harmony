#include "CommandAllocatorPool.h"

#include "Renderer.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    using namespace dx12;

    CommandAllocator& CommandAllocatorPool::requestCommandAllocator(const CommandListType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

        updateAvailableQueue(type);

        std::queue<CommandAllocator*>& availableQueue = msAvailableQueue[type];

        if (!availableQueue.empty())
        {
            CommandAllocator* allocator = availableQueue.front();
            availableQueue.pop();
            return *allocator;
        }

        CommandAllocator::Description createDesc;
        createDesc.name = (msCmdAllocatorPool.size() + " type[" + std::to_string(type) + "] command allocator");
        createDesc.type = type;
        msCmdAllocatorPool[type].emplace_back(createDesc);

        return msCmdAllocatorPool[type].back();
    }

    void CommandAllocatorPool::discardCommandAllocator(const uint64_t completedFenceValue, CommandAllocator& usedAllocator)
    {
        const CommandListType type = usedAllocator.getCommandListType();

        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

        msUsedQueue[type].push(std::make_pair(completedFenceValue, &usedAllocator));
    }

    void CommandAllocatorPool::destroyPool()
    {
        for (size_t type = 0; type < CommandListType::CommandListTypeCount; ++type)
        {
            std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

            updateAvailableQueue(CommandListType(type));
            HASSERT(msUsedQueue[type].empty(), "Trying to destroy pool %d while %d allocators are still in used.", type, msUsedQueue[type].size());

            // clear the queues to prevent dangling pointers
            while (!msAvailableQueue[type].empty())
            {
                msAvailableQueue[type].pop();
            }

            msCmdAllocatorPool[type].clear();
        }
    }

    CommandAllocatorPool::~CommandAllocatorPool()
    {
        destroyPool();
    }

    void CommandAllocatorPool::updateAvailableQueue(const CommandListType type)
    {
        while (!msUsedQueue[type].empty() && GfxCommandListManager().isFenceComplete(msUsedQueue[type].front().first))
        {
            msAvailableQueue[type].push(msUsedQueue[type].front().second);
            msUsedQueue[type].pop();
        }
    }
}
