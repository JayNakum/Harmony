#pragma once

#include "common.h"
#include "directx.h"

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
            Fence(const std::string& name);

            void wait(uint64_t fenceValue, DWORD miliSeconds = INFINITE);

            ~Fence();

            // Returns the raw pointer of ID3D12Fence
            // This is needed because a lot of synchronization functions require the raw pointer.
            inline ID3D12Fence* get() { mFence.Get(); }

            // Operator overload that returns the raw pointer of ID3D12Fence.
            inline ID3D12Fence* operator->()
            {
                return mFence.Get();
            }

        private:
            Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
            std::mutex mFenceEventMutex;
        };
    }
}
