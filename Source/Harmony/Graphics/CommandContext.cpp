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
