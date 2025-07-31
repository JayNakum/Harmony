#include "CommandList.h"

#include "types/CommandListType.h"
#include "types/GPUResourceStates.h"

namespace graphics
{
    CommandList::CommandList(Device& device, Description& createDescription)
        : CommandObject(createDescription.type), mCommandList(nullptr), mResourceBarrierBuffer{}, mNumBarriersToFlush(0)
    {
        HRESULT hResult = device->CreateCommandList(
            1, dx12::toD3DCommandListType(createDescription.type),
            (createDescription.allocator).get(), nullptr, IID_PPV_ARGS(&mCommandList));

        HASSERT(SUCCEEDED(hResult), "Failed to create command list %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

        setName(createDescription.name);
    }

    void CommandList::setName(const std::string name)
    {
        setD3DName(mCommandList.Get(), utility::toWideString(name).c_str());
    }

    void CommandList::transitionBarrierFull(GPUResource& resource, const GPUResourceStates newState, const bool flushImmediate)
    {
        GPUResourceStates oldState = resource.getUsageState();
        if (oldState != newState)
        {
            HASSERT(mNumBarriersToFlush < CommandList::MAX_RESOURCE_BARRIERS, "Exeeded maximum limit on buffered barriers.");
            D3D12_RESOURCE_BARRIER& barrier = mResourceBarrierBuffer[mNumBarriersToFlush++];

            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource = resource.get();
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = dx12::toD3DResourceStates(oldState);
            barrier.Transition.StateAfter = dx12::toD3DResourceStates(newState);
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

            resource.setTransitioningState((GPUResourceStates)-1);
            resource.setUsageState(newState);
        }

        if (flushImmediate || mNumBarriersToFlush == CommandList::MAX_RESOURCE_BARRIERS)
        {
            flushResourceBarriers();
        }
    }

    void CommandList::uavBarrier(GPUResource& resource, const bool flushImmediate)
    {
        HASSERT(mNumBarriersToFlush < CommandList::MAX_RESOURCE_BARRIERS, "Exeeded maximum limit on buffered barriers.");
        D3D12_RESOURCE_BARRIER& barrier = mResourceBarrierBuffer[mNumBarriersToFlush++];
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.UAV.pResource = resource.get();

        if (flushImmediate || mNumBarriersToFlush == CommandList::MAX_RESOURCE_BARRIERS)
        {
            flushResourceBarriers();
        }
    }

    void CommandList::aliasBarrier(GPUResource& resourceBefore, GPUResource& resourceAfter, const bool flushImmediate)
    {
        HASSERT(mNumBarriersToFlush < CommandList::MAX_RESOURCE_BARRIERS, "Exeeded maximum limit on buffered barriers.");
        D3D12_RESOURCE_BARRIER& barrier = mResourceBarrierBuffer[mNumBarriersToFlush++];

        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Aliasing.pResourceBefore = resourceBefore.get();
        barrier.Aliasing.pResourceAfter = resourceAfter.get();

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
