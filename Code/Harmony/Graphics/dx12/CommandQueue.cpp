#include "CommandQueue.h"

#include "types/CommandListType.h"

namespace graphics
{
    CommandQueue::CommandQueue(Device& device, const Description& createDescription)
        : CommandObject(createDescription.type),
        mCommandQueue(nullptr),
        mFence(device, createDescription.name + "fence"),
        mNextFenceValue(1),
        mLastCompletedFenceValue(0)
    {
        D3D12_COMMAND_QUEUE_DESC createDesc{};
        createDesc.Type = dx12::toD3DCommandListType(mType);
        createDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        HRESULT hResult = device->CreateCommandQueue(&createDesc, IID_PPV_ARGS(&mCommandQueue));
        HASSERT(SUCCEEDED(hResult), "Failed to create command queue %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

        setD3DName(mCommandQueue.Get(), utility::toWideString(createDescription.name).c_str());

        // Encode type at the top bit of the fence.
        mFence->Signal(0);
    }

    const uint64_t CommandQueue::incrementFence()
    {
        std::lock_guard<std::mutex> scopedLockGuard(mFenceMutex);
        return incrementFenceNoMutex();
    }

    bool CommandQueue::isFenceComplete(const uint64_t fenceValue)
    {
        // If check to avoid querying the fence value by testing against the last one seen.
        // The max() is to protect against an unlikely race condition
        // that could cause the last completed fence value to regress.
        if (fenceValue > mLastCompletedFenceValue)
        {
            mLastCompletedFenceValue = std::max(mLastCompletedFenceValue, mFence->GetCompletedValue());
        }

        return fenceValue <= mLastCompletedFenceValue;
    }

    void CommandQueue::waitForFence(const uint64_t fenceValue)
    {
        if (isFenceComplete(fenceValue))
        {
            return;
        }
        mFence.wait(fenceValue);
        mLastCompletedFenceValue = fenceValue;
    }

    void CommandQueue::stallForFence(Fence fence)
    {
        mCommandQueue->Wait(fence.get(), fence->GetCompletedValue());
    }

    void CommandQueue::stallForQueue(const CommandQueue& queue)
    {
        stallForFence(queue.mFence);
    }

    const uint64_t CommandQueue::executeCommandLists(CommandList commandLists[], const unsigned int count)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mFenceMutex);

        std::vector<ID3D12CommandList*> d3dCommandLists;
        d3dCommandLists.reserve(count);
        for (unsigned int i = 0; i < count; ++i)
        {
            d3dCommandLists.push_back(commandLists[i].get());
        }

        mCommandQueue->ExecuteCommandLists((UINT)d3dCommandLists.size(), d3dCommandLists.data());

        return incrementFenceNoMutex();
    }

    /*const uint64_t CommandQueue::executeCommandLists(CommandList commandList[], const size_t count)
    {
        std::lock_guard<std::mutex> scopedLockGuard(mFenceMutex);

        std::vector<ID3D12CommandList*> d3dLists(count);
        for (unsigned int i = 0; i < count; ++i)
        {
            d3dLists.push_back(commandList[i].get());
        }

        mCommandQueue->ExecuteCommandLists(count, d3dLists.data());
        return incrementFenceNoMutex();
    }*/

    const uint64_t CommandQueue::incrementFenceNoMutex()
    {
        mCommandQueue->Signal(mFence.get(), mNextFenceValue);
        return mNextFenceValue++;
    }

    CommandQueue::~CommandQueue()
    {
        mCommandQueue = nullptr;
    }
}
