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
        )
    {}

    DescriptorHeapWithAllocator::Page DynamicDescriptorHeap::allocatePage()
    {
        return mCPUHeap.allocatePage();
    }

    dx12::DescriptorHandle DynamicDescriptorHeap::finalize(const DescriptorHeapWithAllocator::Page& cpuPage)
    {
        DescriptorHeapWithAllocator::Page gpuPage = mGPUHeap.allocatePage();

        mGPUHeap.copyDescriptorsFrom(mDevice, gpuPage.getStartHandle(), { cpuPage.getStartHandle() }, { (unsigned int)(cpuPage.getUsedHandlesCount()) });

        return gpuPage.getStartHandle();
    }

    DynamicDescriptorHeap::~DynamicDescriptorHeap()
    {
    }
}
