#include "CommandContextPool.h"

namespace graphics
{
    GraphicContextPool::GraphicContextPool(const CommandListType type)
        : CommandObject(type)
    {}

    GraphicContext* GraphicContextPool::requestContext()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        if (!mAvailableQueue.empty())
        {
            GraphicContext* context = mAvailableQueue.front();
            mAvailableQueue.pop();
            context->reset();

            return context;
        }

        msGraphicContextPool.emplace_back();
        return &(msGraphicContextPool.back());
    }

    void GraphicContextPool::discardContext(GraphicContext& context)
    {
        const CommandListType type = context.getCommandListType();
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        mAvailableQueue.push(&context);
    }

    void GraphicContextPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        // clear the queue to prevent dangling pointers
        while (!mAvailableQueue.empty())
        {	
            mAvailableQueue.pop();
        }

        GraphicContextPool::msGraphicContextPool.clear();
    }

    GraphicContextPool::~GraphicContextPool()
    {
        destroyPool();
    }
}
