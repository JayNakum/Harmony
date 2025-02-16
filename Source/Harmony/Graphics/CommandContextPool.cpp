#include "CommandContextPool.h"

namespace graphics
{
    CommandContextPool::CommandContextPool(const CommandListType type)
        : CommandObject(type)
    {}

    CommandContext& CommandContextPool::requestContext()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        std::queue<CommandContext*>& availableQueue = mAvailableQueue;

        if (!availableQueue.empty())
        {
            CommandContext* context = availableQueue.front();
            availableQueue.pop();
            context->reset();

            return *context;
        }

        mCommandContextPool.emplace_back();
        return mCommandContextPool.back();
    }

    void CommandContextPool::discardContext(CommandContext& context)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        CommandContextPool::mAvailableQueue.push(&context);
    }

    void CommandContextPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        // clear the queue to prevent dangling pointers
        while (!mAvailableQueue.empty())
        {	
            mAvailableQueue.pop();
        }

        CommandContextPool::mCommandContextPool.clear();
    }

    CommandContextPool::~CommandContextPool()
    {
        destroyPool();
    }
}
