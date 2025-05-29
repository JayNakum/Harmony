#pragma once

#include "common.h"

#include "CommandContext.h"

namespace graphics
{
    /*
    * Context Pool manages the memory for creating and destroying Command Contexts
    * It has a pool of contexts and manages its allocation and deallocation.
    */
    class CommandContextPool
    {
        static constexpr unsigned int MAX_CONTEXTS = 16;
    public:
        CommandContextPool() = default;

        // Allocates a new context for the given command list type.
        // If an available context exists, it is reset and reused.
        // Otherwise, a new one is created and initialized.
        GraphicContext& requestGraphicContext(dx12::Device& device, DynamicDescriptorHeap& shaderViewsDescriptorHeap, DescriptorHeapWithAllocator& renderTargetViewsCPUDescriptorHeap);

        // Puts the context in available queue for future use.
        // This doesn't delete or reset the context. The context is reset first when requested via allocateContext().
        void discardContext(GraphicContext& context);

        // Empties out the queue and destroys the entire pool.
        void destroyPool();

        ~CommandContextPool() { destroyPool(); }

    private:
        // The main context pool containing all the contexts of each type.
        std::vector<GraphicContext*> msGraphicContextPool;
        // A queue of contexts of each type that are available for reuse.
        std::queue<GraphicContext*> mGraphicAvailableQueue;
        // Prevents incorrect reading and writing of context pool and available queue.
        std::mutex mGraphicPoolMutex;
    };
}
