#pragma once

#include "common.h"
#include "Log.h"

#include "CommandContext.h"

namespace graphics
{
    /*
    * Context Pool manages the memory for creating and destroying Command Contexts
    * It has a pool of contexts and manages its allocation and deallocation.
    * Allocates a new context for the given command list type.
    * If an available context exists, it is reset and reused.
    * Otherwise, a new one is created and initialized.
    */
    class CommandContextPool
    {
        static constexpr unsigned int MAX_CONTEXTS = 16;
    public:
        CommandContextPool() = default;

        CopyContext& requestCopyContext(Device& device);
        ComputeContext& requestComputeContext(Device& device);
        GraphicContext& requestGraphicContext(Device& device);

        // Puts the context in available queue for future use.
        // This doesn't delete or reset the context. The context is reset first when requested via allocateContext().
        void discardContext(CopyContext& context);
        void discardContext(ComputeContext& context);
        void discardContext(GraphicContext& context);

        // Empties out the queue and destroys the entire pool.
        void destroyPool();

        ~CommandContextPool() { destroyPool(); }

    private:
        // The main copy context pool containing all the contexts of each type.
        std::vector<CopyContext*> mCopyContextPool;
        // A queue of copy contexts of each type that are available for reuse.
        std::queue<CopyContext*> mCopyAvailableQueue;
        // Prevents incorrect reading and writing of copy context pool and available queue.
        std::mutex mCopyPoolMutex;

        // The main compute context pool containing all the contexts of each type.
        std::vector<ComputeContext*> mComputeContextPool;
        // A queue of compute contexts of each type that are available for reuse.
        std::queue<ComputeContext*> mComputeAvailableQueue;
        // Prevents incorrect reading and writing of compute context pool and available queue.
        std::mutex mComputePoolMutex;

        // The main graphics context pool containing all the contexts of each type.
        std::vector<GraphicContext*> mGraphicContextPool;
        // A queue of graphics contexts of each type that are available for reuse.
        std::queue<GraphicContext*> mGraphicAvailableQueue;
        // Prevents incorrect reading and writing of graphics context pool and available queue.
        std::mutex mGraphicPoolMutex;
    };
}
