#include "CommandContextPool.h"

namespace graphics
{
    CopyContext& CommandContextPool::requestCopyContext(Device& device)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mCopyPoolMutex);

        HASSERT(mCopyContextPool.size() <= MAX_CONTEXTS, "Copy Context Pool Budget exceeded.");

        if (!mCopyAvailableQueue.empty())
        {
            CopyContext* context = mCopyAvailableQueue.front();
            mCopyAvailableQueue.pop();
            return *context;
        }

        mCopyContextPool.emplace_back(new CopyContext(device));
        return *(mCopyContextPool.back());
    }

    ComputeContext& CommandContextPool::requestComputeContext(Device& device)
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

    GraphicContext& CommandContextPool::requestGraphicContext(Device& device)
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

    void CommandContextPool::discardContext(CopyContext& context)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mCopyPoolMutex);

        mCopyAvailableQueue.push(&context);
    }

    void CommandContextPool::discardContext(ComputeContext& context)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mComputePoolMutex);

        mComputeAvailableQueue.push(&context);
    }

    void CommandContextPool::discardContext(GraphicContext& context)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mGraphicPoolMutex);

        mGraphicAvailableQueue.push(&context);
    }

    void CommandContextPool::destroyPool()
    {
        {
            std::lock_guard<std::mutex> scopedLockGuard(mCopyPoolMutex);

            // clear the queue to prevent dangling pointers
            while (!mCopyAvailableQueue.empty())
            {
                mCopyAvailableQueue.pop();
            }

            for (const CopyContext* context : mCopyContextPool)
            {
                delete context;
            }
            CommandContextPool::mCopyContextPool.clear();
        }
        {
            std::lock_guard<std::mutex> scopedLockGuard(mComputePoolMutex);

            // clear the queue to prevent dangling pointers
            while (!mComputeAvailableQueue.empty())
            {
                mComputeAvailableQueue.pop();
            }

            for (const ComputeContext* context : mComputeContextPool)
            {
                delete context;
            }
            CommandContextPool::mComputeContextPool.clear();
        }
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
}
