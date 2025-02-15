#pragma once

namespace graphics
{
    // Type of resources in the heap.
    enum DescriptorHeapType
    {
        View, // CBV_SRV_UAV
        Sampler,
        RenderTarget,
        DepthStencil,

        DescriptorHeapTypeCount
    };

    // Descriptor heap visibility.
    enum DescriptorHeapFlag
    {
        None, // CPU Only
        ShaderVisible, // CPU and GPU both

        DescriptorHeapFlagCount
    };
}
