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

    inline static const std::string toString(const DescriptorHeapType e)
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
        None = 0, // CPU Only
        GPUVisible = 1 << 0 // shader visible
    };

    inline static const DescriptorHeapFlags operator|(DescriptorHeapFlags a, DescriptorHeapFlags b)
    {
        return static_cast<DescriptorHeapFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline static const DescriptorHeapFlags& operator|=(DescriptorHeapFlags& a, DescriptorHeapFlags b)
    {
        a = a | b;
        return a;
    }
    inline static const DescriptorHeapFlags operator&(DescriptorHeapFlags a, DescriptorHeapFlags b)
    {
        return static_cast<DescriptorHeapFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline static const bool hasFlag(DescriptorHeapFlags flags, DescriptorHeapFlags check)
    {
        return (flags & check) != DescriptorHeapFlags::None;
    }

    inline static const std::string toString(const DescriptorHeapFlags e)
    {
        switch (e)
        {
        default:
            return "";
        case DescriptorHeapFlags::None:
            return "None";
        case DescriptorHeapFlags::GPUVisible:
            return "GPUVisible";
        }
    }
}
