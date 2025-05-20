#include "CommandList.h"

#include "types/CommandListType.h"
#include "types/GPUResourceStates.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        CommandList::CommandList(Device& device, Description& createDescription)
            : CommandObject(createDescription.type), mCommandList(nullptr), mResourceBarrierBuffer{}, mNumBarriersToFlush(0)
        {
            HRESULT hResult = device->CreateCommandList(
                1, toD3DCommandListType(createDescription.type),
                (createDescription.allocator).get(), nullptr, IID_PPV_ARGS(&mCommandList));

            HASSERT(SUCCEEDED(hResult), "Failed to create command list %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            setName(createDescription.name);
        }

        void CommandList::setName(const std::string name)
        {
            setD3DName(mCommandList.Get(), utility::toWideString(name).c_str());
        }

        void CommandList::transitionResource(GPUResource& resource, const GPUResourceStates newState, bool flushImmediate)
        {
            GPUResourceStates oldState = resource.getUsageState();
            if (oldState != newState)
            {
                HASSERT(mNumBarriersToFlush < CommandList::MAX_RESOURCE_BARRIERS, "Exeeded maximum limit on buffered barriers.");
                D3D12_RESOURCE_BARRIER& barrier = mResourceBarrierBuffer[mNumBarriersToFlush++];

                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Transition.pResource = resource.get();
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrier.Transition.StateBefore = toD3DResourceStates(oldState);
                barrier.Transition.StateAfter = toD3DResourceStates(newState);

                // Check if we already started the transition
                if (newState == resource.getTransitioningState())
                {
                    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
                    resource.setTransitioningState(GPUResourceStates::Common);
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
            /*HRESULT hResult = mCommandList->Close();
            HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", utility::translateErrorCode(hResult));*/
            mCommandList = nullptr;
        }
    }
}
