#pragma once

#include "common.h"

#include "CommandObject.h"

#include "dx12/CommandList.h"
#include "dx12/DescriptorHeap.h"
#include "dx12/GPUResource.h"

#include "GPUResourceEnums.h"
#include "UploadBuffer.h"
#include "GPUMemory.h"


namespace graphics
{

    /*
    * This is the classes that connects graphics classes with DX12.
    * Contains the state and resources needed to record and execute commands on the GPU.
    * This parent class is inherited by GraphicsContext and // TODO: ComputeContext.
    */
    class CommandContext : public CommandObject, NonCopyable
    {

    protected:
        // Command context is only ever meant to be created, initialized or reset via the Context Manager
        CommandContext(const CommandListType type);

        void bindDescriptorHeaps() const;

    public:
        inline void setID(const std::string& id) { mID = id; }

        inline dx12::CommandList& getCommandList() const { return *mCommandList; }

        // Sets the descriptor in the array of currentDescriptorHeaps.
        // currentDescriptorHeaps are then bind to GPU before draw call.
        void setDescriptorHeap(dx12::DescriptorHeap& descriptorHeap);

        uint64_t finish(bool waitForCompletion = false);
        void reset();

        virtual void deallocate() = 0;
        virtual ~CommandContext();

    public:
        // Static methods that use the default command context.

        static void initializeBuffer(GPUMemory& dest, const void* data, size_t numBytes, size_t destOffset = 0);
        static void initializeBuffer(GPUMemory& dest, UploadBuffer& src, size_t srcOffset = 0, size_t numBytes = -1, size_t destOffset = 0);

    private:
        std::string mID;
        dx12::CommandList* mCommandList;
        dx12::CommandAllocator* mCurrentCommandAllocator;

        std::array<dx12::DescriptorHeap*, descriptorHeapTypeCount> mCurrentDescriptorHeaps;
    };
}

namespace graphics
{
    class GraphicContextPool;

    class GraphicContext : public CommandContext
    {
    public:
        GraphicContext();
        static GraphicContext& begin(const std::string& id = "DEFAULT");

        void deallocate() override;

    private:
        static GraphicContextPool msPool;
    };
}
