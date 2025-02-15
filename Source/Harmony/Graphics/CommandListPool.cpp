#include "CommandListPool.h"

namespace graphics
{
    CommandListPool::CommandListPool(const CommandListType type)
        : CommandObject(type)
    {}

    dx12::CommandList& CommandListPool::requestCommandList(dx12::CommandAllocator& allocator)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        dx12::CommandList::Description createDesc{
            mCommandListPool.size() + " command list",
            mType,
            allocator
        };

        mCommandListPool.emplace_back(createDesc);

        return mCommandListPool.back();
    }

    CommandListPool::~CommandListPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mPoolMutex);

        mCommandListPool.clear();
    }
}
