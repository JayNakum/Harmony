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
    enum class DescriptorHeapFlag
    {
        None = 0, // CPU Only
        GPUVisible = 1 << 0 // shader visible
    };

    inline static const DescriptorHeapFlag operator|(DescriptorHeapFlag a, DescriptorHeapFlag b)
    {
        return static_cast<DescriptorHeapFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline static const DescriptorHeapFlag& operator|=(DescriptorHeapFlag& a, DescriptorHeapFlag b)
    {
        a = a | b;
        return a;
    }
    inline static const DescriptorHeapFlag operator&(DescriptorHeapFlag a, DescriptorHeapFlag b)
    {
        return static_cast<DescriptorHeapFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline static const bool hasFlag(DescriptorHeapFlag flags, DescriptorHeapFlag check)
    {
        return (flags & check) != DescriptorHeapFlag::None;
    }

    inline static const std::string toString(const DescriptorHeapFlag e)
    {
        switch (e)
        {
        default:
            return "";
        case DescriptorHeapFlag::None:
            return "None";
        case DescriptorHeapFlag::GPUVisible:
            return "GPUVisible";
        }
    }
}
