#pragma once

#include "CommandObjectEnums.h"
#include "CommandAllocatorPool.h"

namespace graphics
{
    /*
    * A collection of all the type of CommandAllocatorPools.
    * It is only ever meant to be used via the Renderer.
    */
    class CommandAllocatorManager
    {
        friend class Renderer;

    private:
        // Creates and initializes allocatorPool for each CommandListType
        CommandAllocatorManager();

    public:
        dx12::CommandAllocator& requestAllocator(const CommandListType type);

        void discardAllocator(const uint64_t completedFence, dx12::CommandAllocator& usedAllocator);

    private:
        std::array<CommandAllocatorPool, CommandListType::CommandListTypeCount> mCommandAllocatorPool;
    };
}
