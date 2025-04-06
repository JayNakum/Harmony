#include "CommandQueue.h"

#include "Renderer.h"

#include "types/CommandListType.h"

namespace graphics
{
    namespace dx12
    {
        CommandQueue::CommandQueue(const Description& createDescription)
            : CommandObject(createDescription.type),
            mCommandQueue(nullptr),
            mFence(createDescription.name + "fence"),
            mNextFenceValue((uint64_t)mType << 56 | 1),
            mLastCompletedFenceValue((uint64_t)mType << 56)
        {
            D3D12_COMMAND_QUEUE_DESC createDesc{};
            createDesc.Type = toD3DCommandListType(mType);
            createDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

            HRESULT hResult = GfxDevice()->CreateCommandQueue(&createDesc, IID_PPV_ARGS(&mCommandQueue));
            HASSERT_HR(hResult, "Failed to create command queue %s.", createDescription.name);

#if defined(RELEASE)
            (void)createDescription.name;
#else
            mCommandQueue->SetName(utility::toWideString(createDescription.name).c_str());
#endif
            // Encode type at the top bit of the fence.
            mFence->Signal((uint64_t)mType << 56);
        }

        uint64_t CommandQueue::incrementFence()
        {
            std::lock_guard<std::mutex> scoppedLockGuard(mFenceMutex);
            return incrementFenceNoMutex();
        }

        bool CommandQueue::isFenceComplete(uint64_t fenceValue)
        {
            // If check to avoid querying the fence value by testing against the last one seen.
            // The max() is to protect against an unlikely race condition that could cause the last
            // completed fence value to regress.
            if (fenceValue > mLastCompletedFenceValue)
                mLastCompletedFenceValue = std::max(mLastCompletedFenceValue, mFence->GetCompletedValue());

            return fenceValue <= mLastCompletedFenceValue;
        }

        void CommandQueue::waitForFence(uint64_t fenceValue)
        {
            if (isFenceComplete(fenceValue))
            {
                return;
            }
            mFence.wait(fenceValue);
            mLastCompletedFenceValue = fenceValue;
        }


        CommandQueue::~CommandQueue()
        {
            waitForIdle();
            mCommandQueue = nullptr;
        }

        uint64_t CommandQueue::incrementFenceNoMutex()
        {
            mCommandQueue->Signal(mFence.get(), mNextFenceValue);
            return mNextFenceValue++;
        }

        uint64_t CommandQueue::executeCommandList(dx12::CommandList& commandList)
        {
            std::lock_guard<std::mutex> scoppedLockGuard(mFenceMutex);

            HRESULT hResult = commandList->Close();
            HASSERT_HR(hResult, "Failed to close command list.");

            ID3D12CommandList* list = commandList.get();
            mCommandQueue->ExecuteCommandLists(1, &list);

            return incrementFenceNoMutex();
        }
    }
}
