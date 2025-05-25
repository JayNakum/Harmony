#include "DynamicDescriptorHeap.h"

namespace graphics
{
    DynamicDescriptorHeap::DynamicDescriptorHeap(dx12::Device& device)
        : mDevice(device),
        mCPUHeap(
            mDevice,
            {
                "cpu visible shader view descriptor heap",
                graphics::DescriptorHeapType::ShaderViews,
                MAX_DESCRIPTORS_COUNT,
                graphics::DescriptorHeapFlags::CPUVisible
            }
        ),
        mGPUHeap(
            mDevice,
            {
                "gpu visible shader view descriptor heap",
                graphics::DescriptorHeapType::ShaderViews,
                MAX_DESCRIPTORS_COUNT,
                graphics::DescriptorHeapFlags::GPUVisible
            }
        ),
        mCurrentGPUPage(mGPUHeap.allocatePage())
    {}

    DescriptorHeapWithAllocator::Page DynamicDescriptorHeap::allocatePage()
    {
        return mCPUHeap.allocatePage();
    }

    dx12::DescriptorHandle DynamicDescriptorHeap::finalize(const dx12::DescriptorHandle& allocationStart, const unsigned int descriptorsCount)
    {
        if (descriptorsCount <= 0)
        {
            return dx12::DescriptorHandle();
        }

        if (!mCurrentGPUPage.canAllocate(descriptorsCount))
        {
            mCurrentGPUPage = mGPUHeap.allocatePage();
        }

        dx12::DescriptorHandle allocationHandle = mCurrentGPUPage.allocate(descriptorsCount);

        mGPUHeap.copyDescriptorsFrom(mDevice, allocationHandle, { allocationStart }, { descriptorsCount });

        return allocationHandle;
    }
}
