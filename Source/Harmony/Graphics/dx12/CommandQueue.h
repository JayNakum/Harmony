#pragma once

#include "common.h"
#include "directx.h"

#include "Device.h"

#include "CommandObject.h"

#include "CommandList.h"
#include "Fence.h"

namespace graphics
{
    namespace dx12
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

            struct ExecuteCommandListsParams
            {
                Fence const* fencesToWait;
                unsigned int fenceToWaitCount;

				CommandList const* commandLists;
                unsigned int commandListCount;
				
                Fence const* fencesToSignal;
				unsigned int fenceToSignalCount;
            };

            // Executes multiple commandlist and returns the incremented fence.
            const uint64_t executeCommandLists(ExecuteCommandListsParams& executeCommandListsParams);

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
        };
    }
}
