#include "CommandListPool.h"

namespace graphics
{
    CommandListPool::CommandListPool(const CommandListType type)
        : CommandObject(type)
    {}

    dx12::CommandList& CommandListPool::requestCommandList(dx12::CommandAllocator& allocator)
    {
        dx12::CommandList::Description createDesc{
            mCommandListPool.size() + " command list",
            mType,
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
