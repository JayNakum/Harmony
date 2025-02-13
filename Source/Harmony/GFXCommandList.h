#pragma once

#include "common.h"

#include "directx.h"

namespace graphics
{
    enum CommandListType
    {
        Direct,
        Compute,
        // TODO: add support for Copy and Bundle type command lists

        CommandListTypeCount
    };

    /*
    * Contains the common functions and variables that are needed by all the different command objects.
    */
    class CommandObject
    {
    protected:
        // Initializes the mType
        CommandObject(const CommandListType type)
            : mType(type) {}

    public:
        inline const CommandListType getCommandListType() const { return mType; }

    protected:
        const CommandListType mType;
    };

    namespace dx12
    {
        /*
        *  A wrapper over ID3D12CommandAllocator
        */
        class CommandAllocator : public CommandObject
        {
            friend class CommandAllocatorPool;
        public:
            struct Description
            {
                std::string name;
                CommandListType type;
            };

            // Creates an ID3D12CommandAllocator of type createDescription.type
            CommandAllocator(const Description& createDescription);

            // Destroys the ID3D12CommandAllocator
            ~CommandAllocator();

            // Returns the raw pointer on ID3D12CommandAllocator
            inline ID3D12CommandAllocator* get() { return mCommandAllocator.Get(); }

            // Operator overload that returns the raw pointer of ID3D12CommandAllocator.
            inline ID3D12CommandAllocator* operator->()
            {
                return mCommandAllocator.Get();
            }

        private:
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
        };
    }

    namespace dx12
    {
        /*
        * Wrapper over ID3D12Fence
        */
        class Fence
        {
        public:
            // Creates an ID3D12Fence
            Fence(const std::string& name);

            void wait(uint32_t fenceValue, DWORD miliSeconds = INFINITE);

            ~Fence();

            // Returns the raw pointer of ID3D12Fence
            // This is needed because a lot of synchronization functions require the raw pointer.
            inline ID3D12Fence* get() { mFence.Get(); }

            // Operator overload that returns the raw pointer of ID3D12Fence.
            inline ID3D12Fence* operator->()
            {
                return mFence.Get();
            }

        private:
            Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
            std::mutex mFenceEventMutex;
        };
    }

    namespace dx12
    {
        /*
        * Wrapper over ID3D12CommandList
        */
        class CommandList : public CommandObject
        {
            friend class CommandListManager;
        public:
            struct Description
            {
                std::string name;
                CommandListType type;
                CommandAllocator allocator;
            };

            // Creates an ID3D12CommandList.
            CommandList(Description& createDesc);

            // Destroys the command list.
            // Doesn't wait or reset anything, straight up frees the ComPtr
            ~CommandList();

            // Returns the raw pointer to ID3D12CommandList required for ExecuteCommandList
            inline ID3D12CommandList* get() { return mCommandList.Get(); }

            // Returns the raw pointer to ID3D12GraphicsCommandList
            inline ID3D12GraphicsCommandList* operator->()
            {
                return mCommandList.Get();
            }

        public:
            static const std::unordered_map<graphics::CommandListType, D3D12_COMMAND_LIST_TYPE> commandListType;

        private:
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
        };
    }

    namespace dx12
    {
        /*
        * Wrapper over ID3D12CommandQueue
        */
        class CommandQueue : public CommandObject
        {
            friend class CommandListManager;
        public:
            struct Description
            {
                std::string name;
                CommandListType type;
            };

            // Creates an ID3D12CommandQueue of createDescription.type.
            CommandQueue(const Description& createDescription);

            // Executes the commandlist and returns the incremented fence.
            uint64_t executeCommandList(dx12::CommandList& commandList);

            // Signals the command queue and returns the incremented fence value.
            uint64_t incrementFence();

            // Checks if the fenceValue is reached or not.
            bool isFenceComplete(uint64_t fenceValue);

            // Waits for the passed fence value.
            void waitForFence(uint64_t fenceValue);

            // Flushes the entire command queue.
            inline void waitForIdle() { waitForFence(incrementFence()); }

            // Flushes the queueu and destroys the ID3D12CommadQueue.
            ~CommandQueue();

            // Operator overload that returns the raw pointer of ID3D12CommandQueue.
            inline ID3D12CommandQueue* operator->()
            {
                return mCommandQueue.Get();
            }

        private:
            // Increments the fence WITHOUT locking the mutex
            // This method is used internally to prevent double locking the mutex.
            // Locking the mutex twice leads to a deadlock.
            uint64_t incrementFenceNoMutex();

        private:
            Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;

            // For CPU-GPU synchronization
            Fence mFence;
            uint64_t mNextFenceValue;
            uint64_t mLastCompletedFenceValue;
            std::mutex mFenceMutex;
        };
    }
}
