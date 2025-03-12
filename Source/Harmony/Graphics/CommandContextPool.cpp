#include "CommandContextPool.h"

namespace graphics
{
    CommandContextPool::CommandContextPool(const CommandListType type)
        : CommandObject(type)
    {}

    dx12::CommandContext* CommandContextPool::requestContext()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex);

        std::queue<dx12::CommandContext*>& availableQueue = msAvailableQueue;

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
            msGraphicContextPool.emplace_back();
            return &(msGraphicContextPool.back());
        }
        }
    }

    void CommandContextPool::discardContext(dx12::CommandContext& context)
    {
        const CommandListType type = context.getCommandListType();
        std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex);

        CommandContextPool::msAvailableQueue.push(&context);
    }

    void CommandContextPool::destroyPool()
    {
        for (size_t type = 0; type < CommandListType::CommandListTypeCount; ++type)
        {
            std::lock_guard<std::mutex> scoppedLockGuard(msPoolMutex);

            // clear the queue to prevent dangling pointers
            while (!msAvailableQueue.empty())
            {	
                msAvailableQueue.pop();
            }

        }
        CommandContextPool::msGraphicContextPool.clear();
    }

    CommandContextPool::~CommandContextPool()
    {
        destroyPool();
    }
}
