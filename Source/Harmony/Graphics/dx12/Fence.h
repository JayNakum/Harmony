#pragma once

#include "common.h"
#include "directx.h"

#include "Log.h"

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
            inline const ID3D12Fence* get() const { mFence.Get(); }
            inline ID3D12Fence* get() { return mFence.Get(); }

            // Operator overload that returns the raw pointer of ID3D12Fence.
            inline const ID3D12Fence* operator->() const { return mFence.Get(); }
            inline ID3D12Fence* operator->() { return mFence.Get(); }

        private:
            HANDLE acquireEventHandle();

            void releaseEventHandle(HANDLE& handle, uint64_t fenceValue);

            void destroyAllEventHandles();

        private:
            Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
            std::mutex mFenceEventMutex;

            struct FenceEventCompare
            {
                bool operator()(const std::pair<uint64_t, HANDLE>& a, const std::pair<uint64_t, HANDLE>& b)
                {
                    return a.first > b.first; // Smaller fence value gets higher priority
                }
            };

            std::priority_queue<
                std::pair<uint64_t, HANDLE>,
                std::vector<std::pair<uint64_t, HANDLE>>,
                FenceEventCompare
            > mAvailableEvents;
        };
    }
}
