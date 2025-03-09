#include "CommandContext.h"

#include "Renderer.h"

#include "dx12/GPUResourceStates.h"

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

        uint64_t CommandContext::finish(bool waitForCompletion)
        {
            flushResourceBarriers();

            HASSERT(mCurrentCommandAllocator != nullptr, "Command allocator not found.");

            CommandQueue& queue = GfxCommandListManager().getCommandQueue(mType);
            uint64_t fence = queue.executeCommandList(getCommandList());
            GfxCommandListManager().discardAllocator(fence, *mCurrentCommandAllocator);
            mCurrentCommandAllocator = nullptr;

            if (waitForCompletion)
            {
                GfxCommandListManager().waitForFence(fence);
            }

            GfxCommandContextPool().discardContext(*this);

            return fence;
        }

        void CommandContext::transitionResource(GPUResource& resource, const GPUResourceState newState, bool flushImmediate)
        {
            GPUResourceState oldState = resource.getUsageState();
            if (oldState != newState)
            {
                HASSERT(mNumBarriersToFlush < CommandContext::MAX_RESOURCE_BARRIERS, "Exeeded maximum limit on buffered barriers.");
                D3D12_RESOURCE_BARRIER& barrier = mResourceBarrierBuffer[mNumBarriersToFlush++];

                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Transition.pResource = resource.get();
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrier.Transition.StateBefore = gpuResourceStates.at(oldState);
                barrier.Transition.StateAfter = gpuResourceStates.at(newState);
                // Check to see if we already started the transition
                if (newState == resource.getTransitioningState())
                {
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
                    resource.setTransitioningState(GPUResourceState::Invalid);
                }
                else
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

                resource.setUsageState(newState);
            }

            if (flushImmediate || mNumBarriersToFlush == CommandContext::MAX_RESOURCE_BARRIERS)
            {
                flushResourceBarriers();
            }
        }

        void CommandContext::initializeBuffer(GPUBuffer& dest, const void* data, size_t numBytes, size_t destOffset)
        {
            // Copy data to an intermediate upload heap.
            UploadBuffer intermediateBuffer({"Intermediate Buffer", numBytes});
            {
                void* intermediateDst = intermediateBuffer.map();
                memcpy(intermediateDst, data, numBytes);
                intermediateBuffer.unmap();
            }

            initializeBuffer(dest, intermediateBuffer, numBytes);
        }

        void CommandContext::initializeBuffer(GPUBuffer& dest, const UploadBuffer& src, size_t srcOffset, size_t numBytes, size_t destOffset)
        {
            // Safety check for maximum available bytes
            size_t maxBytes = std::min<size_t>(dest.getSize() - destOffset, src.getSize() - srcOffset);
            HASSERT_SOFT(maxBytes >= numBytes, "Not enough size(%d) in source buffer after offset %d. Copying only %d bytes.", numBytes, srcOffset, maxBytes);
            numBytes = std::min<size_t>(maxBytes, numBytes);

            CommandContext& initContext = GfxContextBegin();

            // Schedule a copy from the upload heap to the default heap.
            initContext.transitionResource(dest, GPUResourceState::CopyDest, true);
            initContext.getCommandList()->CopyBufferRegion(dest.get(), destOffset, (ID3D12Resource*)src.get(), 0, numBytes);
            initContext.transitionResource(dest, GPUResourceState::GenericRead, true);

            // Execute the command list and wait for it to finish so we can release the intermediate buffer.
            initContext.finish(true);
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
