#pragma once

#include "common.h"

#include "CommandContext.h"

namespace graphics
{
    /*
    * Context Pool manages the memory for creating and destroying Command Contexts
    * It has a pool of contexts and manages its allocation and deallocation.
    */
    class CommandContextPool : public CommandObject
    {
    public:
        CommandContextPool(const CommandListType type)
            : CommandObject(type) {}
        virtual void destroyPool() = 0;
        virtual ~CommandContextPool() { destroyPool(); }
    };

    class GraphicContextPool : public CommandContextPool
    {
    public:
        GraphicContextPool(const CommandListType type);

        // Allocates a new context for the given command list type.
        // If an available context exists, it is reset and reused.
        // Otherwise, a new one is created and initialized.
        GraphicContext& requestContext();

        // Puts the context in available queue for future use.
        // This doesn't delete or reset the context. The context is reset first when requested via allocateContext().
        void discardContext(GraphicContext& context);

        // Empties out the queue and destroys the entire pool.
        void destroyPool() override;

        ~GraphicContextPool();

    private:
        // The main context pool containing all the contexts of each type.
        std::vector<GraphicContext> msGraphicContextPool;

        // A queue of contexts of each type that are available for reuse.
        std::queue<GraphicContext*> mAvailableQueue;

        // Prevents incorrect reading and writing of context pool and available queue.
        std::mutex mPoolMutex;
    };
}
