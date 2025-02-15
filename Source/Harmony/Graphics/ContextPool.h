#pragma once

#include "CommandObjectEnums.h"
#include "CommandContext.h"

namespace graphics
{
    /*
    * Context Pool manages the memory for creating and destroying Command Contexts
    * It has a pool of contexts and manages its allocation and deallocation.
    */
    class ContextPool
    {
    public:
        // Allocates a new context for the given command list type.
        // If an available context exists, it is reset and reused.
        // Otherwise, a new one is created and initialized.
        CommandContext& requestContext(CommandListType type);

        // Puts the context in available queue for future use.
        // This doesn't delete or reset the context. The context is reset first when requested via allocateContext().
        void freeContext(CommandContext& context);

        // Empties out the queues and destroys the entire pool.
        void destroyPool();

        ~ContextPool();

    private:
        // The main context pool containing all the contexts of each type.
        std::vector<CommandContext> mContextPool[CommandListType::CommandListTypeCount];

        // A queue of contexts of each type that are available for reuse.
        std::queue<CommandContext*> mAvailableQueue[CommandListType::CommandListTypeCount];

        // Prevents incorrect reading and writing of context pool and available queue.
        std::mutex mPoolMutex;
    };
}
