#include "CommandContextPool.h"

namespace graphics
{
    GraphicContext& CommandContextPool::requestGraphicContext()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mGraphicPoolMutex);

        if (!mGraphicAvailableQueue.empty())
        {
            GraphicContext* context = mGraphicAvailableQueue.front();
            mGraphicAvailableQueue.pop();
            return *context;
        }

        msGraphicContextPool.emplace_back();
        return (msGraphicContextPool.back());
    }

    void CommandContextPool::discardContext(GraphicContext& context)
    {
        const CommandListType type = context.getCommandListType();
        std::lock_guard<std::mutex> scoppedLockGuard(mGraphicPoolMutex);

        mGraphicAvailableQueue.push(&context);
    }

    void CommandContextPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mGraphicPoolMutex);

        // clear the queue to prevent dangling pointers
        while (!mGraphicAvailableQueue.empty())
        {
            mGraphicAvailableQueue.pop();
        }

        CommandContextPool::msGraphicContextPool.clear();
    }
}
