#pragma once

#include "common.h"

#include "CommandListManager.h"

#include "CommandObject.h"

#include "dx12/CommandList.h"
#include "dx12/DescriptorHeap.h"
#include "dx12/GPUResource.h"
#include "dx12/UploadBuffer.h"
#include "dx12/GPUBuffer.h"

namespace graphics
{
    /*
    * This is the classes that connects graphics classes with DX12.
    * Contains the state and resources needed to record and execute commands on the GPU.
    * This parent class is inherited by GraphicsContext and // TODO: ComputeContext.
    */
    class CommandContext : public CommandObject//, public NonCopyable
    {
    protected:
        // Command context is only ever meant to be created, initialized or reset via the Context Manager
        CommandContext(const CommandListType type, CommandListManager& commandListManager);

        void bindDescriptorHeaps() const;

        inline void setName(const std::string& name) { mName = name; }

    public:
        inline dx12::CommandList& getCommandList() const { return *mCommandList; }

        // Sets the descriptor in the array of currentDescriptorHeaps.
        // currentDescriptorHeaps are then bind to GPU before draw call.
        void setDescriptorHeap(dx12::DescriptorHeap& descriptorHeap);

        uint64_t finish(const bool waitForCompletion = false);
        void reset();

        virtual void deallocate() = 0;
        virtual ~CommandContext();

    private:
        std::string mName;
        dx12::CommandList* mCommandList;
        dx12::CommandAllocator* mCurrentCommandAllocator;

        CommandListManager& mCommandListManager;

        std::array<dx12::DescriptorHeap*, descriptorHeapTypeCount> mCurrentDescriptorHeaps;
    };
}

namespace graphics
{
    class GraphicContext : public CommandContext
    {
    public:
        GraphicContext(CommandListManager& commandListManager);

        void deallocate() override;
    };
}
