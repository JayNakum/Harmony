#include "CommandContextPool.h"

namespace graphics
{
    CommandContext& CommandContextPool::requestContext(const CommandListType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

        std::queue<CommandContext*>& availableQueue = msAvailableQueue[type];

        if (!availableQueue.empty())
        {
            CommandContext* context = availableQueue.front();
            availableQueue.pop();
            context->reset();

            return *context;
        }

        msCommandContextPool[type].emplace_back();
        return msCommandContextPool[type].back();
    }

    void CommandContextPool::discardContext(CommandContext& context)
    {
        const CommandListType type = context.getCommandListType();
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

        CommandContextPool::msAvailableQueue[type].push(&context);
    }

    void CommandContextPool::destroyPool()
    {
        for (size_t type = 0; type < CommandListType::CommandListTypeCount; ++type)
        {
            std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

            // clear the queue to prevent dangling pointers
            while (!msAvailableQueue[type].empty())
            {	
                msAvailableQueue[type].pop();
            }

            CommandContextPool::msCommandContextPool[type].clear();
        }
    }

    CommandContextPool::~CommandContextPool()
    {
        destroyPool();
    }
}
