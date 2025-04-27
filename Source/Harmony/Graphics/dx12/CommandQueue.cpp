#include "CommandQueue.h"

#include "types/CommandListType.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        CommandQueue::CommandQueue(Device& device, const Description& createDescription)
            : CommandObject(createDescription.type),
            mCommandQueue(nullptr),
            mFence(device, createDescription.name + "fence"),
            mNextFenceValue((uint64_t)mType << 56 | 1),
            mLastCompletedFenceValue((uint64_t)mType << 56)
        {
            D3D12_COMMAND_QUEUE_DESC createDesc{};
            createDesc.Type = toD3DCommandListType(mType);
            createDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

            HRESULT hResult = device->CreateCommandQueue(&createDesc, IID_PPV_ARGS(&mCommandQueue));
            HASSERT(SUCCEEDED(hResult), "Failed to create command queue %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            setD3DName(mCommandQueue.Get(), utility::toWideString(createDescription.name).c_str());

            // Encode type at the top bit of the fence.
            mFence->Signal((uint64_t)mType << 56);
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

        const uint64_t CommandQueue::executeCommandList(dx12::CommandList& commandList)
        {
            std::lock_guard<std::mutex> scopedLockGuard(mFenceMutex);

            HRESULT hResult = commandList->Close();
            HASSERT(SUCCEEDED(hResult), "Failed to close command list.\n%s", utility::translateErrorCode(hResult));

            ID3D12CommandList* list = commandList.get();
            mCommandQueue->ExecuteCommandLists(1, &list);

            return incrementFenceNoMutex();
        }

        /*const uint64_t CommandQueue::executeCommandLists(dx12::CommandList commandList[], const size_t count)
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
}
