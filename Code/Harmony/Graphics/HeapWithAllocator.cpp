#include "HeapWithAllocator.h"

namespace graphics
{
    HeapWithAllocator::HeapWithAllocator(dx12::Device& device)
        : mDevice(device),
        mUploadHeap(
            mDevice,
            {
                "HeapWithAllocator_UploadHeap",
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::Upload],
                HeapType::Upload
            }),
        mDefaultHeap(
            mDevice,
            {
                "HeapWithAllocator_DefaultHeap",
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::Transient] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::Constants] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::Mesh] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::Texture] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::Persistent],
                HeapType::Default
            })
    {}

    dx12::ByteAddressBuffer HeapWithAllocator::createBuffer(const ResourceDescription& createDescription)
    {
        dx12::ByteAddressBuffer buffer(createDescription.buffer);
        buffer.createPlaced(
            mDevice,
            getBackingHeap(createDescription.category),
            mAllocator.allocate(createDescription.category, buffer.getAllocationInfo(mDevice).sizeInBytes)
        );
        return buffer;
    }

    dx12::Texture HeapWithAllocator::createTexture(const ResourceDescription& createDescription)
    {
        dx12::Texture texture(createDescription.texture);
        texture.createPlaced(
            mDevice,
            getBackingHeap(createDescription.category),
            mAllocator.allocate(createDescription.category, texture.getAllocationInfo(mDevice).sizeInBytes)
        );
        return texture;
    }

    dx12::RenderTarget HeapWithAllocator::createRenderTarget(dx12::Texture& texture, const dx12::RenderTarget::Type type)
    {
        return dx12::RenderTarget(type, texture);
    }

    dx12::RenderTarget HeapWithAllocator::createRenderTarget(const ResourceDescription& createDescription, const dx12::RenderTarget::Type type)
    {
        return createRenderTarget(createTexture(createDescription), type);
    }
}
