#include "CommandListPool.h"

namespace graphics
{
    dx12::CommandList& CommandListPool::requestCommandList(const CommandListType type, dx12::CommandAllocator& allocator)
    {
        dx12::CommandList::Description createDesc{
            mCommandListPool.size() + " " + toString(type) + " type command list",
            type,
            allocator
        };

        mCommandListPool.emplace_back(createDesc);

        return mCommandListPool.back();
    }

    void CommandListPool::destroyPool()
    {
        mCommandListPool.clear();
    }

    CommandListPool::~CommandListPool()
    {
        destroyPool();
    }
}
