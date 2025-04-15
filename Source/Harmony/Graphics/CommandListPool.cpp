#include "CommandListPool.h"

namespace graphics
{
    dx12::CommandList& CommandListPool::requestCommandList(dx12::Device& device, const CommandListType type, dx12::CommandAllocator& allocator)
    {
        dx12::CommandList::Description createDesc{
            mCommandListPool.size() + " " + toString(type) + " type command list",
            type,
            allocator
        };

        mCommandListPool.emplace_back(device, createDesc);

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
