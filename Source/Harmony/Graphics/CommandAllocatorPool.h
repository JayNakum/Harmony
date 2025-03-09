#pragma once

#include "common.h"

#include "dx12/CommandAllocator.h"

namespace graphics
{
    /*
    * Manages a thread safe memory pool of command allocators.
    */
    class CommandAllocatorPool
    {
    public:
        // TODO: Cache in a couple allocators already and put them in available queue
        CommandAllocatorPool() = default;

        // Provides a CommandAllocator from the available queue.
        // If none are available then creates a new one.
        dx12::CommandAllocator& requestCommandAllocator(const CommandListType type);

        // Discards the allocator after GPU is done with it.
        // The fence value represents the last completed GPU work that used this allocator.
        void discardCommandAllocator(const uint64_t completedFenceValue, dx12::CommandAllocator& usedAllocator);

        // Empties out the queues and destroys the entire pool.
        void destroyPool();

        ~CommandAllocatorPool();

    private:
        // Goes through the used queue and puts all the allocators that have completed their fence into available queue.
        void updateAvailableQueue(const CommandListType type);

    private:
        // The memory pool containing all the CommandAllocators for each type.
        static std::array<std::vector<dx12::CommandAllocator>, CommandListTypeCount> msCmdAllocatorPool;

        // Once an allocator is discarded, it is kept in the used queue.
        static std::array<std::queue<std::pair<uint64_t, dx12::CommandAllocator*>>, CommandListTypeCount> msUsedQueue;

        // Once the fence value of an allocator inside used queue is reached, ie gpu is done with the allocator,
        // it is put inside the available queue ready for reuse.
        static std::array<std::queue<dx12::CommandAllocator*>, CommandListTypeCount> msAvailableQueue;

        // Prevents incorrect read and write of the pool and queue
        static std::array<std::mutex, CommandListTypeCount> msPoolMutex;
    };
}
