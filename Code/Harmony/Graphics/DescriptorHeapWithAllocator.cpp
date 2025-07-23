#include "DescriptorHeapWithAllocator.h"

#include "dx12/types/DescriptorHeapType.h"

namespace graphics
{
    DescriptorHeapWithAllocator::DescriptorHeapWithAllocator(dx12::Device& device)
        : mDevice(device),
        mShaderViewsDescriptorHeapCPU(
            mDevice,
            {
                "DynamicDescriptorHeap_ShaderViewsCPU",
                DescriptorHeapType::ShaderViews,
                DescriptorAllocator::MEMORY_BUDGET[DescriptorsCategory::DynamicCPU] / mDevice.getDescriptorHandleIncrementSize(DescriptorHeapType::ShaderViews),
                DescriptorHeapFlags::CPUVisible
            }
        ),
        mShaderViewsDescriptorHeapGPU(
            mDevice,
            {
                "DynamicDescriptorHeap_ShaderViewsGPU",
                DescriptorHeapType::ShaderViews,
                DescriptorAllocator::MEMORY_BUDGET[DescriptorsCategory::DynamicGPU] / mDevice.getDescriptorHandleIncrementSize(DescriptorHeapType::ShaderViews),
                DescriptorHeapFlags::GPUVisible
            }
        ),
        mRenderTargetViewsDescriptorHeap(
            mDevice,
            {
                "DynamicDescriptorHeap_RenderTargetViewsCPU",
                DescriptorHeapType::RenderTarget,
                DescriptorAllocator::MEMORY_BUDGET[DescriptorsCategory::RenderTargetCPU] / mDevice.getDescriptorHandleIncrementSize(DescriptorHeapType::RenderTarget),
                DescriptorHeapFlags::CPUVisible
            }
        )
    {}

    dx12::DescriptorHandle DescriptorHeapWithAllocator::allocate(const DescriptorsCategory category, const size_t count)
    {
        const size_t allocationSize = count * mDevice.getDescriptorHandleIncrementSize(DescriptorHeapType::ShaderViews);
        const size_t allocationOffset = mAllocator.allocate(category, allocationSize);

        return getDescriptorHeap(category).getHeapStart() + (INT)allocationOffset;
    }

    void DescriptorHeapWithAllocator::copyDescriptorsFrom(
        dx12::Device& device,
        const dx12::DescriptorHandle& destStart,
        const std::vector<dx12::DescriptorHandle>& srcStarts,
        const std::vector<unsigned int>& srcCounts,
        const DescriptorsCategory category)
    {
        getDescriptorHeap(category).copyDescriptorsFrom(device, destStart, srcStarts, srcCounts);
    }
}
