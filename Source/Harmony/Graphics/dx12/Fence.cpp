#include "Fence.h"

#include "Core.h"

namespace graphics
{
    namespace dx12
    {
        Fence::Fence(const std::string& name)
            : mFence(nullptr)
        {
            HRESULT hResult = GfxDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
            HASSERT_HR(hResult, "Failed to create fence.");
            setName(mFence.Get(), utility::toWideString(name).c_str());
        }

        void Fence::wait(uint64_t fenceValue)
        {
            // Passing NULL is thread safe and doesn't require complex fenceEvent management.
            // Especially for this case where we need to wait immediately.
            HRESULT hResult = mFence->SetEventOnCompletion(fenceValue, NULL);
            HASSERT_HR(hResult, "Failed to set event on fence completion and wait.");
        }

        Fence::~Fence()
        {
            mFence = nullptr;
        }
    }
}
