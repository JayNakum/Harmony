#pragma once

#include "common.h"

#include "dx12/Device.h"

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
    * It is only supposed to be used via Core.
    */
    class CommandListManager
    {
    public:
        // Creates and initializes the internal CommandQueues.
        // TODO: Add compute and copy queue support.
        CommandListManager(dx12::Device& device);

        ~CommandListManager();

        // Returns the CommandQueue if the specified type.
        inline dx12::CommandQueue& getCommandQueue(const CommandListType type) { return getQueue(type).commandQueue; }
        inline dx12::CommandQueue& getGraphicsQueue() { return getCommandQueue(CommandListType::Direct); }

        inline dx12::CommandAllocator& requestAllocator(const CommandListType type) { return getQueue(type).requestAllocator(mDevice); }
        inline void discardAllocator(dx12::CommandAllocator& usedAllocator) { getQueue(usedAllocator.getCommandListType()).discardAllocator(usedAllocator); }

    public:
        struct CommandObjects
        {
            dx12::CommandList commandList;
            dx12::CommandAllocator commandAllocator;
        };
        CommandObjects createNewCommandList(const CommandListType type);

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
            Queue(dx12::Device& device, const CommandListType type)
                : commandQueue(device, {"global " + toString(type) + " command queue", type}),
                commandAllocatorPool(type) {}

            dx12::CommandQueue commandQueue;

            // The CommandAllocatorPool is NOT thread_local
            // because if each thread has its own pool, one thread may have free allocators while another runs out.
            CommandAllocatorPool commandAllocatorPool;

            inline dx12::CommandAllocator& requestAllocator(dx12::Device& device) { return commandAllocatorPool.requestCommandAllocator(device, commandQueue.getCompletedFence()); }
            inline void discardAllocator(dx12::CommandAllocator& usedAllocator) { commandAllocatorPool.discardCommandAllocator(commandQueue.getCompletedFence(), usedAllocator); }
        };

        Queue& getQueue(const CommandListType type);

        Queue mGraphicsQueue;

        dx12::Device& mDevice;

        // Command Lists are thread local.
        // This allows us to have multiple threads writing commands in parallel.
        // We can still achieve that with some mutex locks inside the Pool
        // but command lists are not meant to be destroyed frequently
        // since there will be ideally only one command list per thread, having this thread local makes more sense.
        thread_local static CommandListPool msCommandListPool;
    };
}
