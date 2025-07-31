#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "Device.h"

#include "CommandObject.h"

#include "CommandList.h"
#include "Fence.h"

namespace graphics
{
    /*
    * Wrapper over ID3D12CommandQueue
    */
    class CommandQueue : public CommandObject
    {
    public:
        struct Description
        {
            std::string name;
            CommandListType type;
        };

        // Creates an ID3D12CommandQueue of createDescription.type.
        CommandQueue(Device& device, const Description& createDescription);

        // Signals the command queue and returns the incremented fence value.
        const uint64_t incrementFence();

        // Checks if the fenceValue is reached or not.
        bool isFenceComplete(const uint64_t fenceValue);

        // CPU Waits for the passed fence value.
        void waitForFence(const uint64_t fenceValue);

        // GPU Waits for the passed fence value.
        void stallForFence(Fence fence);
        void stallForQueue(const CommandQueue& queue);

        // Flushes the entire command queue.
        inline void waitForIdle() { waitForFence(incrementFence()); }

        // Executes the commandlist and returns the incremented fence.
        const uint64_t executeCommandLists(CommandList commandList[], const unsigned int count = 1);

        // Executes multiple commandlists and returns the incremented fence.
        // const uint64_t executeCommandLists(CommandList commandList[], const size_t count = 1);

        // Flushes the queue and destroys the ID3D12CommadQueue.
        ~CommandQueue();

        // Returns the raw pointer of ID3D12CommandQueue.
        inline const ID3D12CommandQueue* get() const { return mCommandQueue.Get(); }
        inline ID3D12CommandQueue* get() { return mCommandQueue.Get(); }

        // Operator overload that returns the raw pointer of ID3D12CommandQueue.
        inline const ID3D12CommandQueue* operator->() const { return mCommandQueue.Get(); }
        inline ID3D12CommandQueue* operator->() { return mCommandQueue.Get(); }

    private:
        // Increments the fence WITHOUT locking the mutex
        // This method is used internally to prevent double locking the mutex.
        // Locking the mutex twice leads to a deadlock.
        const uint64_t incrementFenceNoMutex();

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;

        // For CPU-GPU synchronization
        Fence mFence;
        uint64_t mNextFenceValue;
        uint64_t mLastCompletedFenceValue;
        std::mutex mFenceMutex;
    };
}
