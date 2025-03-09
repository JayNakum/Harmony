#pragma once

#include "CommandAllocatorPool.h"
#include "CommandListPool.h"

#include "dx12/CommandQueue.h"

namespace graphics
{
    /*
    * Creates a command lists of any type on request.
    * Contains the CommandQueues and also has the synchronization functions.
    * It is only supposed to be used via Renderer.
    */
    class CommandListManager
    {
        friend class Renderer;
    private:
        // Creates and initializes the internal CommandQueues.
        // TODO: Add compute and copy queue support.
        CommandListManager();

        ~CommandListManager();

    public:
        // Returns the CommandQueue if the specified type.
        dx12::CommandQueue& getCommandQueue(const CommandListType type);
        inline dx12::CommandQueue& getGraphicsQueue() { return getCommandQueue(CommandListType::Direct); }

        inline dx12::CommandAllocator& requestAllocator(const CommandListType type) { return msCommandAllocatorPool.requestCommandAllocator(type); }
        inline void discardAllocator(uint64_t fence, dx12::CommandAllocator& usedAllocator) { msCommandAllocatorPool.discardCommandAllocator(fence, usedAllocator); }

    public:
        void createNewCommandList(const CommandListType type, dx12::CommandList*& outCommandList, dx12::CommandAllocator*& outCommandAllocator);

        // Test to see if a fence has already been reached
        inline bool isFenceComplete(uint64_t fenceValue)
        {
            getCommandQueue(CommandListType(fenceValue >> 56)).isFenceComplete(fenceValue);
        }

        // The CPU will wait for a fence to reach a specified value
        inline void waitForFence(uint64_t fenceValue)
        {
            getCommandQueue(CommandListType(fenceValue >> 56)).waitForFence(fenceValue);
        }

        inline void idleGPU()
        {
            mGraphicsQueue.waitForIdle();
        }

    private:
        dx12::CommandQueue mGraphicsQueue;

        // The CommandAllocatorPool is NOT thread_local
        // because if each thread has its own pool, one thread may have free allocators while another runs out.
        static CommandAllocatorPool msCommandAllocatorPool;

        // Command Lists are thread local.
        // This allows us to have multiple threads writing commands in parallel.
        // We can still achieve that with some mutex locks inside the Pool (similar to CommandAllocator)
        // but command lists are not meant to be destroyed frequently to cause inefficient usage of them
        // since their will be ideally only one command list per thread, having this thread local makes more sense.
        thread_local static CommandListPool msCommandListPool;
    };
}
