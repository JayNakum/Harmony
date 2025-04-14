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
            HASSERT(SUCCEEDED(hResult), "Failed to create fence.\n%s", utility::translateErrorCode(hResult));
            setName(mFence.Get(), utility::toWideString(name).c_str());
        }

        void Fence::wait(const uint64_t fenceValue)
        {
            // Passing NULL is thread safe and doesn't require complex fenceEvent management.
            // Especially for this case where we need to wait immediately.
            HRESULT hResult = mFence->SetEventOnCompletion(fenceValue, NULL);
            HASSERT(SUCCEEDED(hResult), "Failed to set event on fence completion and wait.\n%s", utility::translateErrorCode(hResult));
        }

        Fence::~Fence()
        {
            mFence = nullptr;
        }
    }
}
