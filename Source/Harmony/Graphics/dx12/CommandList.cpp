#include "CommandList.h"

#include "Renderer.h"

#include "types/CommandListType.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        CommandList::CommandList(Description& createDesc)
            : CommandObject(createDesc.type), mCommandList(nullptr)
        {
            HRESULT hResult = GfxDevice()->CreateCommandList(
                1, commandListType.at(createDesc.type),
                (createDesc.allocator).get(), nullptr, IID_PPV_ARGS(&mCommandList));

            HASSERT_HR(hResult, "Failed to create command list %s", createDesc.name);

#if defined(RELEASE)
            (void)name;
#else
            mCommandList->SetName(utility::toWideString(createDesc.name).c_str());
#endif
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
                barrier.Transition.StateBefore = dx12::gpuResourceStates.at(oldState);
                barrier.Transition.StateAfter = dx12::gpuResourceStates.at(newState);
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

            if (flushImmediate || mNumBarriersToFlush == CommandList::MAX_RESOURCE_BARRIERS)
            {
                flushResourceBarriers();
            }
        }

        CommandList::~CommandList()
        {
            HRESULT hResult = mCommandList->Close();
            HASSERT_HR(hResult, "Failed to close command list.");
            mCommandList = nullptr;
        }
    }
}
