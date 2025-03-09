#pragma once

#include "common.h"

#include "CommandObject.h"

#include "dx12/CommandList.h"
#include "dx12/DescriptorHeap.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Contains the state and resources needed to record and execute commands on the GPU.
        * This is the class that connects outer classes with DX12.
        * This parent class is inherited by GraphicsContext and // TODO: ComputeContext.
        */
        class CommandContext : public CommandObject, NonCopyable
        {
        protected:
            // Command context is only ever meant to be created, initialized or reset via the Context Manager
            CommandContext(const CommandListType type);

            void bindDescriptorHeaps() const;
        public:
            inline CommandList& getCommandList() const { return *mCommandList; }

            // Sets the descriptor in the array of currentDescriptorHeaps.
            // currentDescriptorHeaps are then bind to GPU before draw call.
            void setDescriptorHeap(dx12::DescriptorHeap& descriptorHeap);

            void reset();

            virtual ~CommandContext();

        private:
            CommandList* mCommandList;
            CommandAllocator* mCurrentCommandAllocator;

            std::array<dx12::DescriptorHeap*, DescriptorHeapType::DescriptorHeapTypeCount> mCurrentDescriptorHeaps;
        };
    }
}
