#include "ContextPool.h"

namespace graphics
{

    CommandContext& ContextPool::requestContext(CommandListType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        std::queue<CommandContext*>& availableQueue = mAvailableQueue[type];

        if (!availableQueue.empty())
        {
            CommandContext* context = availableQueue.front();
            availableQueue.pop();
            context->reset();

            return *context;
        }

        mContextPool[type].emplace_back(type);
        return mContextPool[type].back();
    }

    void ContextPool::freeContext(CommandContext& context)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        ContextPool::mAvailableQueue[context.getCommandListType()].push(&context);
    }

    void ContextPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        for (unsigned int i = 0; i < CommandListType::CommandListTypeCount; ++i) // for each type
        {
            // clear the queue to prevent dangling pointers
            while (!mAvailableQueue[i].empty())
            {	
                mAvailableQueue[i].pop();
            }

            ContextPool::mContextPool[i].clear();
        }
    }

    ContextPool::~ContextPool()
    {
        destroyPool();
    }
}
