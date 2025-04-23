#include "DynamicDescriptorHeap.h"

namespace graphics
{
    DynamicDescriptorHeap::DynamicDescriptorHeap(dx12::Device& device)
        :
        mCPUHeap(
            device,
            {
                "cpu visible shader view descriptor heap",
                graphics::DescriptorHeapType::ShaderViews,
                MAX_DESCRIPTORS_COUNT,
                graphics::DescriptorHeapFlags::CPUVisible
            }
        ),
        mGPUHeap(
            device,
            {
                "gpu visible shader view descriptor heap",
                graphics::DescriptorHeapType::ShaderViews,
                MAX_DESCRIPTORS_COUNT,
                graphics::DescriptorHeapFlags::GPUVisible
            }
        )
    {}

    DescriptorHeapWithAllocator::Page DynamicDescriptorHeap::allocateCPUPageForContext()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mCPUHeapMutex);
        return mCPUHeap.allocatePage();
    }

    DynamicDescriptorHeap::~DynamicDescriptorHeap()
    {
    }
}
