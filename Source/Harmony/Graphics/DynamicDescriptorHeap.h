#pragma once

#include "DescriptorHeapWithAllocator.h"

namespace graphics
{
    class DynamicDescriptorHeap
    {
        static constexpr size_t MAX_DESCRIPTORS_COUNT = 65535;

    public:
        DynamicDescriptorHeap(dx12::Device& device);

        DescriptorHeapWithAllocator::Page allocatePage();

        void finalize();

        dx12::DescriptorHandle getGPUHandle(const unsigned int rootIndex);

        ~DynamicDescriptorHeap();

    private:
        DescriptorHeapWithAllocator mCPUHeap;
        DescriptorHeapWithAllocator mGPUHeap;
    };
}
