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
