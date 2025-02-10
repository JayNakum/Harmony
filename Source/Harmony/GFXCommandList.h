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
            ID3D12CommandAllocator* operator->() const
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
            Fence(const std::string& name);


        private:
            Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
        };

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
            ~CommandQueue();

            inline const CommandListType getCommandListType() const { return mType; }

            // Operator overload that returns the raw pointer of ID3D12CommandQueue.
            ID3D12CommandQueue* operator->() const
            {
                return mCommandQueue.Get();
            }

        private:
            const CommandListType mType;
            Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
        };
    }

    class CommandListManager
    {};
}
