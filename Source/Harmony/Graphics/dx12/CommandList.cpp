#include "CommandList.h"

#include "types/CommandListType.h"
#include "types/GPUResourceState.h"

namespace graphics
{
    namespace dx12
    {
        CommandList::CommandList(Description& createDescription)
            : CommandObject(createDescription.type), mCommandList(nullptr), mResourceBarrierBuffer{}, mNumBarriersToFlush(0)
        {
            HRESULT hResult = GfxDevice()->CreateCommandList(
                1, toD3DCommandListType(createDescription.type),
                (createDescription.allocator).get(), nullptr, IID_PPV_ARGS(&mCommandList));

            HASSERT(SUCCEEDED(hResult), "Failed to create command list %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            setName(mCommandList.Get(), utility::toWideString(createDescription.name).c_str());
        }

        void CommandList::transitionResource(GPUResource& resource, const GPUResourceState newState, bool flushImmediate)
        {
            GPUResourceState oldState = resource.getUsageState();
            if (oldState != newState)
            {
                HASSERT(mNumBarriersToFlush < CommandList::MAX_RESOURCE_BARRIERS, "Exeeded maximum limit on buffered barriers.");
                D3D12_RESOURCE_BARRIER& barrier = mResourceBarrierBuffer[mNumBarriersToFlush++];

                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Transition.pResource = resource.get();
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrier.Transition.StateBefore = toD3DResourceState(oldState);
                barrier.Transition.StateAfter = toD3DResourceState(newState);

                // Check if we already started the transition
                if (newState == resource.getTransitioningState())
                {
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
                    resource.setTransitioningState(GPUResourceState::Invalid);
                }
                else
                {
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                }

                resource.setUsageState(newState);
            }

            if (flushImmediate || mNumBarriersToFlush == CommandList::MAX_RESOURCE_BARRIERS)
            {
                flushResourceBarriers();
            }
        }

        void CommandList::flushResourceBarriers()
        {
            if (mNumBarriersToFlush > 0)
            {
                mCommandList->ResourceBarrier(mNumBarriersToFlush, mResourceBarrierBuffer);
                mNumBarriersToFlush = 0;
            }
        }

        CommandList::~CommandList()
        {
            HRESULT hResult = mCommandList->Close();
            HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", utility::translateErrorCode(hResult));
            mCommandList = nullptr;
        }
    }
}
