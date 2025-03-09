#pragma once

#include "CommandObject.h"

#include "dx12/CommandAllocator.h"
#include "dx12/CommandList.h"

namespace graphics
{
    /*
    * This pool doesn't manage a ready queue or anything
    * because, realistically, we never discard a command list anyways. It is always reused.
    */
    class CommandListPool : public CommandObject
    {
    public:
        // TODO: Cache in a couple command lists already and put them in available queue
        CommandListPool(const CommandListType type);

        // Provides a CommandList from the available queue.
        // If none are available then creates a new one.
        dx12::CommandList& requestCommandList(dx12::CommandAllocator& allocator);

        // Clears the pool. Doing which closes each command list and destroys it.
        void destroyPool();

        ~CommandListPool();

    private:
        std::vector<dx12::CommandList> mCommandListPool;
    };
}
