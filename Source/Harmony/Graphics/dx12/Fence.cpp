#include "Fence.h"

#include "Renderer.h"

#include "Utility.h"
#include "Log.h"

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

            // Keeping the event handle local allows a fence to have separate handles for each fence value.
            // Which allows multithreaded waits.
            // If thread A requests a wait on fence 100 and thread B requests a wait on fence 99
            // And the global mEventHandle is already waiting for 100 then thread B will automatically need to wait as well
            // but since the event is local, it solves the problem
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
}
