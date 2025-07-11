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

    dx12::DescriptorHandle DynamicDescriptorHeap::finalize(const dx12::DescriptorHandle& allocationStart, const unsigned int descriptorsCount)
    {
        HASSERT(descriptorsCount > 0, "Cannot cooy 0 descriptors to GPU. descriptorCount = %d.", descriptorsCount);

        if (!mCurrentGPUPage.getAllocator().canAllocate(descriptorsCount * mCPUHeap.getDescriptorSize()))
        {
            mCurrentGPUPage = mGPUHeap.allocatePage();
        }

        dx12::DescriptorHandle allocationHandle = mCurrentGPUPage.allocate(descriptorsCount);

        mGPUHeap.copyDescriptorsFrom(mDevice, allocationHandle, { allocationStart }, { descriptorsCount });

        return allocationHandle;
    }
}
