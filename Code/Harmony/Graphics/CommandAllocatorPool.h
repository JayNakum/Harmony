#pragma once

#include "common.h"

#include "CommandObject.h"

#include "dx12/CommandAllocator.h"

namespace graphics
{
    /*
    * Manages a thread safe memory pool of command allocators.
    */
    class CommandAllocatorPool : public CommandObject
    {
        static constexpr unsigned int MAX_COMMAND_ALLOCATORS = 16;
    public:
        CommandAllocatorPool(const CommandListType type);

        // Provides a CommandAllocator from the available queue.
        // If none are available then creates a new one.
        CommandAllocator& requestCommandAllocator(Device& device, const uint64_t completedFenceValue);

        // Discards the allocator after GPU is done with it.
        // The fence value represents the last completed GPU work that used this allocator.
        void discardCommandAllocator(const uint64_t completedFenceValue, CommandAllocator& usedAllocator);

        // Empties out the queues and destroys the entire pool.
        void destroyPool();

        ~CommandAllocatorPool();

    private:
        // Goes through the used queue and puts all the allocators that have completed their fence into available queue.
        void updateAvailableQueue(const uint64_t completedFenceValue);

    private:
        // The memory pool containing all the CommandAllocators for each type.
        std::vector<CommandAllocator> mCmdAllocatorPool;

        // Once an allocator is discarded, it is kept in the used queue.
        std::queue<std::pair<uint64_t, CommandAllocator*>> mUsedQueue;

        // Once the fence value of an allocator inside used queue is reached, ie gpu is done with the allocator,
        // it is put inside the available queue ready for reuse.
        std::queue<CommandAllocator*> mAvailableQueue;

        // Prevents incorrect read and write of the pool and queue
        std::mutex mPoolMutex;
    };
}
