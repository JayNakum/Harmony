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

    namespace dx12
    {
        /*
        *  A wrapper over ID3D12CommandAllocator
        */
        class CommandAllocator
        {
        public:
            struct Description
            {
                std::string name;
                CommandListType type;
            };

            // Creates an ID3D12CommandAllocator of type createDescription.type
            CommandAllocator(const Description& createDescription);
            ~CommandAllocator();

            inline const CommandListType getCommandListType() const { return mType; }

            // Operator overload that returns the raw pointer of ID3D12CommandAllocator.
            const ID3D12CommandAllocator* operator->() const
            {
                return mCommandAllocator.Get();
            }

        private:
            const CommandListType mType;
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

            inline ID3D12Fence* get() const { mFence.Get(); }

            // Operator overload that returns the raw pointer of ID3D12Fence.
            ID3D12Fence* operator->() const
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
        * Wrapper over ID3D12CommandQueue
        */
        class CommandQueue
        {
        public:
            struct Description
            {
                std::string name;
                CommandListType type;
            };

            // Creates an ID3D12CommandQueue
            CommandQueue(const Description& createDescription);

            uint64_t incrementFence();
            bool isFenceComplete(uint64_t fenceValue);
            void waitForFence(uint64_t fenceValue);

            inline void waitForIdle() { waitForFence(incrementFence()); }

            ~CommandQueue();

            inline const CommandListType getCommandListType() const { return mType; }

            // Operator overload that returns the raw pointer of ID3D12CommandQueue.
            const ID3D12CommandQueue* operator->() const
            {
                return mCommandQueue.Get();
            }

        private:
            const CommandListType mType;
            Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;

            Fence mFence;
            std::mutex mFenceMutex;
            uint64_t mNextFenceValue;
            uint64_t mLastCompletedFenceValue;
        };
    }

    class CommandListManager
    {};
}
