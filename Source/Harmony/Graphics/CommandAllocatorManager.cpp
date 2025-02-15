#include "CommandAllocatorManager.h"

namespace graphics
{
    CommandAllocatorManager::CommandAllocatorManager()
        : mCommandAllocatorPool{
            CommandAllocatorPool(CommandListType::Direct),
            CommandAllocatorPool(CommandListType::Compute),
            CommandAllocatorPool(CommandListType::Copy) }
    {}

    dx12::CommandAllocator& CommandAllocatorManager::requestAllocator(const CommandListType type)
    {
        return mCommandAllocatorPool[type].requestCommandAllocator();
    }

    void CommandAllocatorManager::discardAllocator(const uint64_t completedFence, dx12::CommandAllocator& usedAllocator)
    {
        mCommandAllocatorPool[usedAllocator.getCommandListType()].discardCommandAllocator(completedFence, usedAllocator);
    }
}
