#include "CommandContextPool.h"

namespace graphics
{
    dx12::CommandContext* CommandContextPool::requestContext(const CommandListType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex[type]);

        std::queue<dx12::CommandContext*>& availableQueue = msAvailableQueue[type];

        if (!availableQueue.empty())
        {
            dx12::CommandContext* context = availableQueue.front();
            availableQueue.pop();
            context->reset();

            return context;
        }
        switch (type)
        {
        default:
        case CommandListType::Direct:
        {
            msGraphicContextPool[type].emplace_back();
            return &(msGraphicContextPool[type].back());
        }
        }
    }

    void CommandContextPool::discardContext(dx12::CommandContext& context)
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

        }
        CommandContextPool::msGraphicContextPool.clear();
    }

    CommandContextPool::~CommandContextPool()
    {
        destroyPool();
    }
}
