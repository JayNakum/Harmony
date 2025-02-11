#pragma once

#include "GFXCommandContext.h"
#include "GFXCommandList.h"
#include "GFXDescriptorHeap.h"

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
        CommandContext& allocateContext(CommandListType type);

        // Puts the context in available queue for future use.
        // This doesn't delete or reset the context. The context is reset first when requested via allocateContext().
        void freeContext(CommandContext& context);

        // Destroys all command contexts in the available queue and clears the context pool.
        void destroyAllContexts();

    private:
        // The main context pool containing all the contexts of each type.
        static std::vector<CommandContext*> msContextPool[CommandListType::CommandListTypeCount];

        // A queue of contexts of each type that are available for reuse.
        static std::queue<CommandContext*> msAvailableContexts[CommandListType::CommandListTypeCount];

        // Prevents incorrect reading and writing of context pool and available queue.
        static std::mutex msContextAllocationMutex;
    };
}

namespace graphics
{
    class CommandAllocatorPool
    {
    public:
        CommandAllocatorPool();

    private:
        const CommandListType mType;

        // A non static allocator pool is managed because allocators are local to command queue.
        // We need to have different allocators per queue.
        std::vector<dx12::CommandAllocator> mCmdAllocatorPool;

        // Once an allocator is discarded, it is kept in the used queue.
        std::queue<std::pair<uint64_t, dx12::CommandAllocator>> mUsedCmdAllocators;


        // Once the fence value of an allocator inside used queue is reached, ie gpu is done with the allocator,
        // it is put inside the available queue ready for reuse.
        std::queue<std::pair<uint64_t, dx12::CommandAllocator>> mAvailableCmdAllocators;

        std::mutex mCmdAllocatorMutex;
    };
}

namespace graphics
{
    class DescriptorHeapPool
    {
    private:
        static dx12::DescriptorHeap& requestDescriptorHeap(DescriptorHeapType type);
        static void discardDescriptorHeaps(DescriptorHeapType type, uint64_t fenceValueForReset, const std::vector<DescriptorHeap>& usedHeaps);

    private:
        static const size_t msNumDescriptorsPerHeap = 256;

        // The memory pool containing all the descriptor heaps.
        static std::vector<dx12::DescriptorHeap*> msDescriptorHeapPool[DescriptorHeapType::DescriptorHeapTypeCount];

        // Once a heap is discarded, it is kept in the used queue.
        static std::queue<std::pair<uint64_t, dx12::DescriptorHeap*>> msUsedDescriptorHeaps[DescriptorHeapType::DescriptorHeapTypeCount];

        // Once the fence value of a descriptor inside used queue is reached, ie gpu is done with the heap,
        // it is put inside the available queue ready for reuse.
        static std::queue<dx12::DescriptorHeap*> msAvailableDescriptorHeaps[DescriptorHeapType::DescriptorHeapTypeCount];

        // Prevents incorrect reading and writing of descriptor heap pool, used queue and the available queue.
        static std::mutex msHeapAllocattionMutex;
    };
}
