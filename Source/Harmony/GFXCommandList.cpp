#include "GFXCommandList.h"

#include "GFXManager.h"

#include "Utility.h"
#include "Log.h"

namespace directx
{
    static const std::unordered_map<graphics::CommandListType, D3D12_COMMAND_LIST_TYPE> commandListType = {
        { graphics::CommandListType::Direct, D3D12_COMMAND_LIST_TYPE_DIRECT },
        { graphics::CommandListType::Compute, D3D12_COMMAND_LIST_TYPE_COMPUTE }
    };
}

namespace graphics
{
    namespace dx12
    {
        CommandAllocator::CommandAllocator(const Description & createDescription)
            : mType(createDescription.type)
        {
            HRESULT hResult = GfxDevice()->CreateCommandAllocator(directx::commandListType.at(mType), IID_PPV_ARGS(&mCommandAllocator));
            HASSERT_HR(hResult, "Failed to create command allocator %s.", createDescription.name);
#if defined(RELEASE)
            (void)createDescription.name;
#else
            mCommandAllocator->SetName(utility::toWideString(createDescription.name).c_str());
#endif
        }

        CommandAllocator::~CommandAllocator()
        {
            mCommandAllocator = nullptr;
        }
    }

    namespace dx12
    {
        Fence::Fence(const std::string& name)
        {
            HRESULT hResult = GfxDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
            HASSERT_HR(hResult, "Failed to create fence.");
#if defined(RELEASE)
            (void)name;
#else
            mFence->SetName(utility::toWideString(name).c_str());
#endif
        }

        void Fence::wait(uint32_t fenceValue, DWORD miliSeconds)
        {
            std::lock_guard<std::mutex> scoppedLockGuard(mFenceEventMutex);

            // Keeping the event handle local allows a fence to have separate handles for each fence value.
            // Which allows multithreaded waits.
            // If thread A requests a wait on fence 100 and thread B requests a wait on fence 99
            // And the mEventHandle is already waiting for 100 then thread B will automatically need to wait as well
            HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, NULL);
            HASSERT(eventHandle != NULL, "Failed to create fence event handle.");

            mFence->SetEventOnCompletion(fenceValue, eventHandle);
            WaitForSingleObject(eventHandle, miliSeconds);

            CloseHandle(eventHandle);
        }

        Fence::~Fence()
        {
            mFence = nullptr;
        }
    }

    namespace dx12
    {
        CommandQueue::CommandQueue(const Description& createDescription)
            : mType(createDescription.type), mFence(createDescription.name + "fence")
        {
            D3D12_COMMAND_QUEUE_DESC createDesc{};
            createDesc.Type = directx::commandListType.at(mType);
            createDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

            HRESULT hResult = GfxDevice()->CreateCommandQueue(&createDesc, IID_PPV_ARGS(&mCommandQueue));
            HASSERT_HR(hResult, "Failed to create command queue %s.", createDescription.name);

#if defined(RELEASE)
            (void)createDescription.name;
#else
            mCommandQueue->SetName(utility::toWideString(createDescription.name).c_str());
#endif
        }

        uint64_t CommandQueue::incrementFence()
        {
            std::lock_guard<std::mutex> scoppedLockGuard(mFenceMutex);
            mCommandQueue->Signal(mFence.get(), mNextFenceValue);
            return mNextFenceValue++;
        }

        bool CommandQueue::isFenceComplete(uint64_t fenceValue)
        {
            // Avoid querying the fence value by testing against the last one seen.
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
            mCommandQueue = nullptr;
        }
    }
}
