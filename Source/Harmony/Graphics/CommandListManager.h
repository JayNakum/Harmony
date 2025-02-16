#pragma once

#include "CommandObjectEnums.h"
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

        static CommandListPool msCommandListPool;
    };
}
