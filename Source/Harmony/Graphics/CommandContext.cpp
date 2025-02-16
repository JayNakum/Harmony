#include "CommandContext.h"

#include "Renderer.h"

#include "Log.h"

namespace graphics
{
    CommandContext::CommandContext(const CommandListType type)
        : CommandObject(type), mCommandList(nullptr), mCurrentCommandAllocator(nullptr)
    {
        GfxCommandListManager().createNewCommandList(mType, mCommandList, mCurrentCommandAllocator);
    }

    void CommandContext::bindDescriptorHeaps() const
    {
        std::array<dx12::DescriptorHeap*, DescriptorHeapType::DescriptorHeapTypeCount>& currentDescriptorHeaps = GfxDescriptorHeapManager().getCurrentDescriptorHeaps();

        std::array<ID3D12DescriptorHeap*, DescriptorHeapType::DescriptorHeapTypeCount> heapsToBind{};
        unsigned int nonNullHeaps = 0;
        for (dx12::DescriptorHeap* heap : currentDescriptorHeaps)
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
        GfxDescriptorHeapManager().setDescriptorHeap(descriptorHeap);
    }

    void CommandContext::reset()
    {
        // We only call Reset() on previously freed contexts.
        // The command list persists and will be reused, but we must request a new allocator.
        HASSERT(mCommandList != nullptr, "Command list not found.");
        HASSERT_SOFT(mCurrentCommandAllocator == nullptr, "Command allocator is not null. It will get replaced by a new one.");

        mCurrentCommandAllocator = &(GfxCommandAllocatorManager().requestAllocator(mType));

        getCommandList()->Reset(mCurrentCommandAllocator->get(), nullptr);
    }
}
