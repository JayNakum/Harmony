#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "Device.h"

#include "CommandObject.h"

#include "CommandAllocator.h"
#include "GPUResource.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3D12CommandList
        */
        class CommandList : public CommandObject
        {
            static constexpr unsigned int MAX_RESOURCE_BARRIERS = 16;
        public:
            struct Description
            {
                std::string name;
                CommandListType type;
                CommandAllocator& allocator;
            };

            // Creates an ID3D12CommandList.
            CommandList(Device& device, Description& createDescription);

            void setName(const std::string name);

            // Transition a GPUResource to a newState and optionally flush the barriers immediately.
            void transitionBarrierFull(GPUResource& resource, const GPUResourceStates newState, const bool flushImmediate = false);
            //void transitionBarrierSplitBegin(GPUResource& resource, const GPUResourceStates newState, const bool flushImmediate = false);
            //void transitionBarrierSplitEnd(GPUResource& resource, const GPUResourceStates newState, const bool flushImmediate = false);

            // Ensure that all the UAV access to the GPUResource are complete before any further access can happen.
            void uavBarrier(GPUResource& resource, const bool flushImmediate = false);

            // Ensures that the access on resourceBefore are completed before any further access on resourceAfter can happen.
            // resourceAfter is an alias of resourceBefore.
            void aliasBarrier(GPUResource& resourceBefore, GPUResource& resourceAfter, const bool flushImmediate = false);

            // Flushes the buffered barriers.
            void flushResourceBarriers();

            // Closes and destroys the command list.
            ~CommandList();

            // Returns the raw pointer to ID3D12CommandList required for ExecuteCommandList
            inline const ID3D12CommandList* get() const { return mCommandList.Get(); }
            inline ID3D12CommandList* get() { return mCommandList.Get(); }

            // Returns the raw pointer to ID3D12GraphicsCommandList
            inline const ID3D12GraphicsCommandList* operator->() const { return mCommandList.Get(); }
            inline ID3D12GraphicsCommandList* operator->() { return mCommandList.Get(); }

        private:
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

            D3D12_RESOURCE_BARRIER mResourceBarrierBuffer[MAX_RESOURCE_BARRIERS];
            unsigned int mNumBarriersToFlush;
        };
    }
}
