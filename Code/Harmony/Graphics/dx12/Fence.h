#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "Device.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3D12Fence
        */
        class Fence
        {
        public:
            // Creates an ID3D12Fence
            Fence(Device& device, const std::string& name);

            // Wait for the fence value.
            void wait(const uint64_t fenceValue);

            ~Fence();

            // Returns the raw pointer of ID3D12Fence.
            inline const ID3D12Fence* get() const { mFence.Get(); }
            inline ID3D12Fence* get() { return mFence.Get(); }

            // Operator overload that returns the raw pointer of ID3D12Fence.
            inline const ID3D12Fence* operator->() const { return mFence.Get(); }
            inline ID3D12Fence* operator->() { return mFence.Get(); }

        private:
            Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
        };
    }
}
