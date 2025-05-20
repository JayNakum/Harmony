#include "CommandContextPool.h"

namespace graphics
{
    GraphicContext& CommandContextPool::requestGraphicContext(dx12::Device& device, DynamicDescriptorHeap& descriptorHeap)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mGraphicPoolMutex);

        if (!mGraphicAvailableQueue.empty())
        {
            GraphicContext* context = mGraphicAvailableQueue.front();
            mGraphicAvailableQueue.pop();
            return *context;
        }

        GraphicContext* newContext = new GraphicContext(device, descriptorHeap);
        msGraphicContextPool.emplace_back(newContext);
        return *(newContext);
    }

    void CommandContextPool::discardContext(GraphicContext& context)
    {
        const CommandListType type = context.getCommandListType();
        std::lock_guard<std::mutex> scopedLockGuard(mGraphicPoolMutex);

        mGraphicAvailableQueue.push(&context);
    }

    void CommandContextPool::destroyPool()
    {
        std::lock_guard<std::mutex> scopedLockGuard(mGraphicPoolMutex);

        // clear the queue to prevent dangling pointers
        while (!mGraphicAvailableQueue.empty())
        {
            mGraphicAvailableQueue.pop();
        }

        for (const GraphicContext* context : msGraphicContextPool)
        {
            delete context;
        }
        CommandContextPool::msGraphicContextPool.clear();
    }
}
