#include "CommandAllocatorManager.h"

namespace graphics
{
    std::array<CommandAllocatorPool, CommandListType::CommandListTypeCount> CommandAllocatorManager::msCommandAllocatorPool = {
            CommandAllocatorPool(CommandListType::Direct),
            CommandAllocatorPool(CommandListType::Compute),
            CommandAllocatorPool(CommandListType::Copy)
    };

    CommandAllocatorManager::~CommandAllocatorManager()
    {
        for (CommandAllocatorPool& pool : msCommandAllocatorPool)
        {
            pool.destroyPool(); // We need to explicitely call this because the msCommandAllocatorPool is static
        }
    }

    dx12::CommandAllocator& CommandAllocatorManager::requestAllocator(const CommandListType type)
    {
        return msCommandAllocatorPool[type].requestCommandAllocator();
    }

    void CommandAllocatorManager::discardAllocator(const uint64_t completedFence, dx12::CommandAllocator& usedAllocator)
    {
        msCommandAllocatorPool[usedAllocator.getCommandListType()].discardCommandAllocator(completedFence, usedAllocator);
    }
}
