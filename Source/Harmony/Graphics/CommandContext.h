#pragma once

#include "common.h"

#include "CommandObject.h"

#include "dx12/CommandList.h"
#include "dx12/DescriptorHeap.h"

#include "dx12/GPUBuffer.h"
#include "dx12/UploadBuffer.h"

namespace graphics
{
    /*
    * This is the classes that connects graphics classes with DX12.
    * TODO: Move to DX12 folder when we have multiple APIs.
    */
    namespace dx12
    {
        /*
        * Contains the state and resources needed to record and execute commands on the GPU.
        * This parent class is inherited by GraphicsContext and // TODO: ComputeContext.
        */
        class CommandContext : public CommandObject, NonCopyable
        {
            static constexpr unsigned int MAX_RESOURCE_BARRIERS = 16;

        protected:
            // Command context is only ever meant to be created, initialized or reset via the Context Manager
            CommandContext(const CommandListType type);

            void bindDescriptorHeaps() const;

        public:
            inline void setID(const std::string& id) { mID = id; }

            inline CommandList& getCommandList() const { return *mCommandList; }

            // Sets the descriptor in the array of currentDescriptorHeaps.
            // currentDescriptorHeaps are then bind to GPU before draw call.
            void setDescriptorHeap(dx12::DescriptorHeap& descriptorHeap);

            uint64_t finish(bool waitForCompletion = false);
            void reset();

            void transitionResource(GPUResource& resource, const GPUResourceState newState, bool flushImmediate = false);

            inline void flushResourceBarriers()
            {
                if (mNumBarriersToFlush > 0)
                {
                    getCommandList()->ResourceBarrier(mNumBarriersToFlush, mResourceBarrierBuffer);
                    mNumBarriersToFlush = 0;
                }
            }

            virtual ~CommandContext();

        public:
            // Static methods that use the default command context.

            static void initializeBuffer(GPUBuffer& dest, const void* data, size_t numBytes, size_t destOffset = 0);
            static void initializeBuffer(GPUBuffer& dest, const UploadBuffer& src, size_t srcOffset = 0, size_t numBytes = -1, size_t destOffset = 0);


        private:
            CommandList* mCommandList;
            CommandAllocator* mCurrentCommandAllocator;

            D3D12_RESOURCE_BARRIER mResourceBarrierBuffer[MAX_RESOURCE_BARRIERS];
            UINT mNumBarriersToFlush;

            std::string mID;

            std::array<dx12::DescriptorHeap*, DescriptorHeapType::DescriptorHeapTypeCount> mCurrentDescriptorHeaps;
        };
    }
}
