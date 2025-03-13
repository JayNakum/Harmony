#include "Renderer.h"

namespace graphics
{
    Renderer* Renderer::msInstance = nullptr;

    std::array<DescriptorHeapPool, descriptorHeapTypeCount> Renderer::msDescriptorHeapPool = {
        DescriptorHeapPool({DescriptorHeapType::View}),
        DescriptorHeapPool({DescriptorHeapType::RenderTarget}),
        DescriptorHeapPool({DescriptorHeapType::DepthStencil}),
        DescriptorHeapPool({DescriptorHeapType::Sampler})
    };

    Renderer* Renderer::getInstance()
    {
        if (msInstance)
        {
            return msInstance;
        }

        msInstance = new Renderer();
        return msInstance;
    }
}
