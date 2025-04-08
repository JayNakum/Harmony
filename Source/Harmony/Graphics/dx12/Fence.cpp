#include "Fence.h"

#include "Renderer.h"

namespace graphics
{
    namespace dx12
    {
        Fence::Fence(const std::string& name)
            : mFence(nullptr)
        {
            HRESULT hResult = GfxDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
            HASSERT_HR(hResult, "Failed to create fence.");
#if defined(RELEASE)
            (void)name;
#else
            mFence->SetName(utility::toWideString(name).c_str());
#endif
        }

        void Fence::wait(uint64_t fenceValue, DWORD miliSeconds)
        {
            std::lock_guard<std::mutex> scoppedLockGuard(mFenceEventMutex);
            HANDLE eventHandle = acquireEventHandle();

            HRESULT hResult = mFence->SetEventOnCompletion(fenceValue, eventHandle);
            if (FAILED(hResult))
            {
                releaseEventHandle(eventHandle, fenceValue);
                HASSERT(false, "Failed to set event on fence completion");
            }

            WaitForSingleObject(eventHandle, miliSeconds);
            releaseEventHandle(eventHandle, fenceValue);
        }

        Fence::~Fence()
        {
            std::lock_guard<std::mutex> scoppedLockGuard(mFenceEventMutex);
            destroyAllEventHandles();
            mFence = nullptr;
        }

        HANDLE Fence::acquireEventHandle()
        {
            if (!mAvailableEvents.empty())
            {
                const auto& top = mAvailableEvents.top();
                if (mFence->GetCompletedValue() >= top.first)
                {
                    HANDLE reusedHandle = top.second;
                    mAvailableEvents.pop();
                    return reusedHandle;
                }
            }

            HANDLE newHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            HASSERT(newHandle != NULL, "Failed to create fence event handle");
            return newHandle;
        }

        void Fence::releaseEventHandle(HANDLE& handle, uint64_t fenceValue)
        {
            mAvailableEvents.emplace(fenceValue, &handle);
        }

        void Fence::destroyAllEventHandles()
        {
            while (!mAvailableEvents.empty())
            {
                CloseHandle(mAvailableEvents.top().second);
                mAvailableEvents.pop();
            }
        }
    }
}
