#include "CommandContext.h"

#include "Renderer.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        CommandContext::CommandContext(const CommandListType type)
            : CommandObject(type), mCommandList(nullptr), mCurrentCommandAllocator(nullptr), mCurrentDescriptorHeaps{}
        {
            GfxCommandListManager().createNewCommandList(mType, mCommandList, mCurrentCommandAllocator);
        }

        void CommandContext::bindDescriptorHeaps() const
        {
            std::array<ID3D12DescriptorHeap*, DescriptorHeapType::DescriptorHeapTypeCount> heapsToBind{};
            unsigned int nonNullHeaps = 0;
            for (DescriptorHeap* heap : mCurrentDescriptorHeaps)
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

        void CommandContext::setDescriptorHeap(DescriptorHeap& descriptorHeap)
        {
            DescriptorHeap* currentDescriptorHeap = mCurrentDescriptorHeaps[descriptorHeap.getDescriptorHeapType()];

            HASSERT_SOFT(currentDescriptorHeap != &descriptorHeap, "Trying to set the same descriptor heap again.");

            currentDescriptorHeap = &descriptorHeap;
        }

        void CommandContext::reset()
        {
            // We only call Reset() on previously freed contexts.
            // The command list persists and will be reused, but we must request a new allocator.
            HASSERT(mCommandList != nullptr, "Command list not found.");
            HASSERT_SOFT(mCurrentCommandAllocator == nullptr, "Command allocator is not null. It will get replaced by a new one.");

            mCurrentCommandAllocator = &(GfxCommandListManager().requestAllocator(mType));

            getCommandList()->Reset(mCurrentCommandAllocator->get(), nullptr);
        }
    }
}
