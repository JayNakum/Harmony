#include "CommandContext.h"

#include "Log.h"

namespace graphics
{
    CommandContext::CommandContext(const CommandListType type, CommandListManager& commandListManager)
        : CommandObject(type), mCommandList(nullptr), mCurrentCommandAllocator(nullptr),
         mCommandListManager(commandListManager), mCurrentDescriptorHeaps{}
    {
        mCommandListManager.createNewCommandList(mType, mCommandList, mCurrentCommandAllocator);
    }

    void CommandContext::bindDescriptorHeaps() const
    {
        std::array<ID3D12DescriptorHeap*, descriptorHeapTypeCount> heapsToBind{};
        unsigned int nonNullHeaps = 0;
        for (dx12::DescriptorHeap* heap : mCurrentDescriptorHeaps)
        {
            if (heap != nullptr)
            {
                heapsToBind[nonNullHeaps++] = heap->get();
            }
        }

        if (nonNullHeaps > 0)
        {
            getCommandList()->SetDescriptorHeaps(nonNullHeaps, heapsToBind.data());
        }
    }

    void CommandContext::setDescriptorHeap(dx12::DescriptorHeap& descriptorHeap)
    {
        dx12::DescriptorHeap* currentDescriptorHeap = mCurrentDescriptorHeaps[enum_cast(descriptorHeap.getDescriptorHeapType())];

        HASSERT_SOFT(currentDescriptorHeap != &descriptorHeap, "Trying to set the same descriptor heap again.");

        currentDescriptorHeap = &descriptorHeap;
    }

    uint64_t CommandContext::finish(bool waitForCompletion)
    {
        getCommandList().flushResourceBarriers();

        HASSERT(mCurrentCommandAllocator != nullptr, "Command allocator not found.");

        dx12::CommandQueue& queue = mCommandListManager.getCommandQueue(mType);
        uint64_t fence = queue.executeCommandList(getCommandList());
        mCommandListManager.discardAllocator(*mCurrentCommandAllocator);
        mCurrentCommandAllocator = nullptr;

        if (waitForCompletion)
        {
            mCommandListManager.waitForFence(fence);
        }

        deallocate();

        return fence;
    }

    void CommandContext::initializeBuffer(dx12::GPUBuffer& dest, const void* data, size_t numBytes, const size_t destOffset, GPUResourceState newState)
    {
        //// Copy data to an intermediate upload heap.
        //dx12::UploadBuffer intermediateBuffer({"Intermediate Buffer", numBytes});
        //{
        //    void* intermediateDst = intermediateBuffer.map();
        //    memcpy(intermediateDst, data, numBytes);
        //    intermediateBuffer.unmap();
        //}

        //initializeBuffer(dest, intermediateBuffer, 0, numBytes, 0, newState);
    }

    void CommandContext::initializeBuffer(dx12::GPUBuffer& dest, dx12::UploadBuffer& src, const size_t srcOffset, size_t numBytes, const size_t destOffset, GPUResourceState newState)
    {
        // Safety check for maximum available bytes
        size_t maxBytes = std::min<size_t>(dest.describeResource().width - destOffset, src.getSize() - srcOffset);
        HASSERT(maxBytes >= numBytes, "Not enough size(%d) in source buffer after offset %d. Copying only %d bytes.", numBytes, srcOffset, maxBytes);
        numBytes = std::min<size_t>(maxBytes, numBytes);

        //CommandContext& initContext = GraphicContext::begin(); // TODO: Check if it is faster to switch to copy context.

        //// Schedule a copy from the upload heap to the default heap.
        //initContext.getCommandList().transitionResource(dest, GPUResourceState::CopyDest, true);
        //initContext.getCommandList()->CopyBufferRegion(dest.get(), destOffset, (ID3D12Resource*)src.get(), srcOffset, numBytes);
        //initContext.getCommandList().transitionResource(dest, newState, true);

        //// Execute the command list and wait for it to finish so we can release the intermediate buffer.
        //initContext.finish(true);
    }

    void CommandContext::reset()
    {
        // We only call Reset() on previously freed contexts.
        // The command list persists and will be reused, but we must request a new allocator.
        HASSERT(mCommandList != nullptr, "Command list not found.");
        HASSERT_SOFT(mCurrentCommandAllocator == nullptr, "Command allocator is not null. It will get replaced by a new one.");

        mCurrentCommandAllocator = &(mCommandListManager.requestAllocator(mType));

        getCommandList()->Reset(mCurrentCommandAllocator->get(), nullptr);
    }

    CommandContext::~CommandContext()
    {
    }
}

namespace graphics
{
    GraphicContext::GraphicContext(CommandListManager& commandListManager)
        : CommandContext(CommandListType::Direct, commandListManager) {}

    void GraphicContext::deallocate()
    {
        //GfxCommandContextPool().discardContext(*this);
    }
}
