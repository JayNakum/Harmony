#include "CommandContextPool.h"

namespace graphics
{
    GraphicContext& CommandContextPool::requestGraphicContext(dx12::Device& device)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mGraphicPoolMutex);

        HASSERT(mGraphicContextPool.size() <= MAX_CONTEXTS, "Graphic Context Pool Budget exceeded.");

        if (!mGraphicAvailableQueue.empty())
        {
            GraphicContext* context = mGraphicAvailableQueue.front();
            mGraphicAvailableQueue.pop();
            return *context;
        }

        mGraphicContextPool.emplace_back(new GraphicContext(device));
        return *(mGraphicContextPool.back());
    }

    ComputeContext& CommandContextPool::requestComputeContext(dx12::Device& device)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mComputePoolMutex);

        HASSERT(mComputeContextPool.size() <= MAX_CONTEXTS, "Compute Context Pool Budget exceeded.");

        if (!mComputeAvailableQueue.empty())
        {
            ComputeContext* context = mComputeAvailableQueue.front();
            mComputeAvailableQueue.pop();
            return *context;
        }

        mComputeContextPool.emplace_back(new ComputeContext(device));
        return *(mComputeContextPool.back());
    }

    void CommandContextPool::discardContext(GraphicContext& context)
    {
        const CommandListType type = context.getCommandListType();
        std::lock_guard<std::mutex> scopedLockGuard(mGraphicPoolMutex);

        mGraphicAvailableQueue.push(&context);
    }

    void CommandContextPool::discardContext(ComputeContext& context)
    {
        const CommandListType type = context.getCommandListType();
        std::lock_guard<std::mutex> scopedLockGuard(mComputePoolMutex);

        mComputeAvailableQueue.push(&context);
    }

    void CommandContextPool::destroyPool()
    {
        std::lock_guard<std::mutex> scopedLockGuard(mGraphicPoolMutex);

        // clear the queue to prevent dangling pointers
        while (!mGraphicAvailableQueue.empty())
        {
            mGraphicAvailableQueue.pop();
        }

        for (const GraphicContext* context : mGraphicContextPool)
        {
            delete context;
        }
        CommandContextPool::mGraphicContextPool.clear();
    }
}
