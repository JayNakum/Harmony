#pragma once

#include "CommandAllocatorPool.h"
#include "CommandListPool.h"

#include "dx12/CommandList.h"
#include "dx12/CommandAllocator.h"
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
        inline dx12::CommandQueue& getCommandQueue(const CommandListType type) { return getQueue(type).commandQueue; }
        inline dx12::CommandQueue& getGraphicsQueue() { return getCommandQueue(CommandListType::Direct); }

        inline dx12::CommandAllocator& requestAllocator(const CommandListType type) { return getQueue(type).commandAllocatorPool.requestCommandAllocator(); }
        inline void discardAllocator(uint64_t fence, dx12::CommandAllocator& usedAllocator) { getQueue(usedAllocator.getCommandListType()).commandAllocatorPool.discardCommandAllocator(fence, usedAllocator); }

    public:
        void createNewCommandList(const CommandListType type, dx12::CommandList*& outCommandList, dx12::CommandAllocator*& outCommandAllocator);

        // Test to see if a fence has already been reached
        inline bool isFenceComplete(uint64_t fenceValue)
        {
            return getCommandQueue(CommandListType(fenceValue >> 56)).isFenceComplete(fenceValue);
        }

        // The CPU will wait for a fence to reach a specified value
        inline void waitForFence(uint64_t fenceValue)
        {
            getCommandQueue(CommandListType(fenceValue >> 56)).waitForFence(fenceValue);
        }

        inline void idleGPU()
        {
            mGraphicsQueue.commandQueue.waitForIdle();
        }

    private:
        struct Queue
        {
            Queue(const CommandListType type)
                : commandQueue({"global " + toString(type) + " command queue", type}),
                commandAllocatorPool(type) {}

            dx12::CommandQueue commandQueue;

            // The CommandAllocatorPool is NOT thread_local
            // because if each thread has its own pool, one thread may have free allocators while another runs out.
            CommandAllocatorPool commandAllocatorPool;
        };

        Queue& getQueue(const CommandListType type);

        Queue mGraphicsQueue;

        // Command Lists are thread local.
        // This allows us to have multiple threads writing commands in parallel.
        // We can still achieve that with some mutex locks inside the Pool
        // but command lists are not meant to be destroyed frequently
        // since there will be ideally only one command list per thread, having this thread local makes more sense.
        thread_local static CommandListPool msCommandListPool;
    };
}
