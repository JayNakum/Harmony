#pragma once

#include "common.h"

namespace graphics
{
    // Type of resources in the heap.
    enum class DescriptorHeapType
    {
        ShaderViews, // CBV_SRV_UAV
        Sampler,
        RenderTarget,
        DepthStencil
    };
    constexpr size_t descriptorHeapTypeCount = 4;

    inline static std::string toString(const DescriptorHeapType e)
    {
        switch (e)
        {
        default:
            return "";
        case DescriptorHeapType::ShaderViews:
            return "ShaderViews";
        case DescriptorHeapType::Sampler:
            return "Sampler";
        case DescriptorHeapType::RenderTarget:
            return "RenderTarget";
        case DescriptorHeapType::DepthStencil:
            return "DeptStencil";
        }
    }

    // Descriptor heap visibility.
    enum class DescriptorHeapFlags
    {
        CPUVisible, // CPU Only
        GPUVisible // shader visible
    };

    inline static std::string toString(const DescriptorHeapFlags e)
    {
        switch (e)
        {
        default:
            return "";
        case DescriptorHeapFlags::CPUVisible:
            return "CPUVisible";
        case DescriptorHeapFlags::GPUVisible:
            return "GPUVisible";
        }
    }
}
