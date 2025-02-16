#pragma once

#include "common.h"

#include "DescriptorHeapEnums.h"
#include "DescriptorHeapPool.h"

namespace graphics
{
    /*
    * A collection of all the type of DescriptorHeapManager.
    * It is only ever meant to be used via the Renderer.
    */
    class DescriptorHeapManager
    {
        friend class Renderer;
    private:
        DescriptorHeapManager();
        ~DescriptorHeapManager();

    public:
        dx12::DescriptorHeap& requestDescriptorHeap(const DescriptorHeapType type);

        void discardDescriptorHeap(const uint64_t completedFence, dx12::DescriptorHeap& usedDescriptorHeap);

        // Sets the descriptor in the array of currentDescriptorHeaps.
        // currentDescriptorHeaps are then bind to GPU before draw call.
        void setDescriptorHeap(dx12::DescriptorHeap& setDescriptorHeap);

        inline std::array<dx12::DescriptorHeap*, DescriptorHeapType::DescriptorHeapTypeCount>& getCurrentDescriptorHeaps()
        {
            return mCurrentDescriptorHeaps;
        }

    private:
        std::array<dx12::DescriptorHeap*, DescriptorHeapType::DescriptorHeapTypeCount> mCurrentDescriptorHeaps;

        static std::array<DescriptorHeapPool, DescriptorHeapType::DescriptorHeapTypeCount> msDescriptorHeapPool;
    };
}
