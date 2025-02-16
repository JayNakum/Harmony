#include "DescriptorHeapManager.h"

#include "Log.h"

namespace graphics
{
    std::array<DescriptorHeapPool, DescriptorHeapType::DescriptorHeapTypeCount> DescriptorHeapManager::msDescriptorHeapPool = {
            DescriptorHeapPool({DescriptorHeapType::View}),
            DescriptorHeapPool({DescriptorHeapType::Sampler}),
            DescriptorHeapPool({DescriptorHeapType::RenderTarget}),
            DescriptorHeapPool({DescriptorHeapType::DepthStencil})
    };

    DescriptorHeapManager::DescriptorHeapManager()
        : mCurrentDescriptorHeaps{}
    {
    }

    DescriptorHeapManager::~DescriptorHeapManager()
    {
        for (DescriptorHeapPool& pool : msDescriptorHeapPool)
        {
            pool.destroyPool();// We need to explicitely call this because the msDescriptorHeapPool is static
        }
    }

    dx12::DescriptorHeap& DescriptorHeapManager::requestDescriptorHeap(const DescriptorHeapType type)
    {
        return msDescriptorHeapPool[type].requestDescriptorHeap();
    }

    void DescriptorHeapManager::discardDescriptorHeap(const uint64_t completedFence, dx12::DescriptorHeap& usedDescriptorHeap)
    {
        msDescriptorHeapPool[usedDescriptorHeap.getDescriptorHeapType()].discardDescriptorHeap(completedFence, usedDescriptorHeap);
    }

    void DescriptorHeapManager::setDescriptorHeap(dx12::DescriptorHeap& setDescriptorHeap)
    {
        dx12::DescriptorHeap* currentDescriptorHeap = mCurrentDescriptorHeaps[setDescriptorHeap.getDescriptorHeapType()];
        HASSERT_SOFT(currentDescriptorHeap == nullptr, "Descriptor heap of type %d already set. Overwriting the previous one.", setDescriptorHeap.getDescriptorHeapType());

        currentDescriptorHeap = &setDescriptorHeap;
    }
}

