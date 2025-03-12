#pragma once

#include "common.h"

namespace graphics
{
    // Type of resources in the heap.
    enum class DescriptorHeapType
    {
        View, // CBV_SRV_UAV
        Sampler,
        RenderTarget,
        DepthStencil
    };
    constexpr size_t descriptorHeapTypeCount = 4;

    const std::string toString(const DescriptorHeapType e)
    {
        switch (e)
        {
        default:
            return "";
        case DescriptorHeapType::View:
            return "View";
        case DescriptorHeapType::Sampler:
            return "Sampler";
        case DescriptorHeapType::RenderTarget:
            return "RenderTarget";
        case DescriptorHeapType::DepthStencil:
            return "DeptStencil";
        }
    }

    // Descriptor heap visibility.
    enum class DescriptorHeapFlag
    {
        CPUVisible, // CPU Only
        GPUVisible // CPU and GPU both
    };

    const std::string toString(const DescriptorHeapFlag e)
    {
        switch (e)
        {
        default:
            return "";
        case DescriptorHeapFlag::CPUVisible:
            return "CPUVisible";
        case DescriptorHeapFlag::GPUVisible:
            return "GPUVisible";
        }
    }
}
