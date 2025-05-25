#pragma once

#include "DescriptorHeapWithAllocator.h"

namespace graphics
{
    class DynamicDescriptorHeap
    {
        static constexpr size_t MAX_DESCRIPTORS_COUNT = 65536;

    public:
        DynamicDescriptorHeap(dx12::Device& device);

        inline const DescriptorHeapWithAllocator& getCPUHeap() const { return mCPUHeap; }
        inline const DescriptorHeapWithAllocator& getGPUHeap() const { return mGPUHeap; }

        // Allocates a page from CPU heap
        // So that each thread can have its own page from the cpu heap
        DescriptorHeapWithAllocator::Page allocatePage();

        // Copies the descriptors from cpuPage to GPU heap
        dx12::DescriptorHandle finalize(const dx12::DescriptorHandle& allocationStart, const unsigned int descriptorsCount);

        ~DynamicDescriptorHeap() = default;

    private:
        dx12::Device& mDevice;
        DescriptorHeapWithAllocator mCPUHeap;
        DescriptorHeapWithAllocator mGPUHeap;

        DescriptorHeapWithAllocator::Page mCurrentGPUPage;
    };
}
