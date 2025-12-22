#include "Fence.h"

namespace graphics
{
    Fence::Fence(Device& device, const std::string& name)
        : mFence(nullptr)
    {
        HRESULT hResult = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
        HASSERT(SUCCEEDED(hResult), "Failed to create fence.\n", utility::translateErrorCode(hResult).c_str());
        setD3DName(mFence.Get(), utility::toWideString(name).c_str());
    }

    void Fence::wait(const uint64_t fenceValue)
    {
        // Passing NULL is thread safe and doesn't require complex fenceEvent management.
        // Especially for this case where we need to wait immediately.
        HRESULT hResult = mFence->SetEventOnCompletion(fenceValue, nullptr);
        HASSERT(SUCCEEDED(hResult), "Failed to set event on fence completion and wait.\n", utility::translateErrorCode(hResult).c_str());
    }

    Fence::~Fence()
    {
        mFence = nullptr;
    }
}
