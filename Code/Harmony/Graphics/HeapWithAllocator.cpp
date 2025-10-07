#include "HeapWithAllocator.h"

namespace graphics
{
    HeapWithAllocator::HeapWithAllocator(Device& device)
        : mDevice(device),
        mUploadHeap(
            mDevice,
            {
                "HeapWithAllocator_UploadHeap",
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::UploadData] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::UploadPersistentData],
                HeapType::Upload
            }),
        mDefaultHeap(
            mDevice,
            {
                "HeapWithAllocator_DefaultHeap",
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::TransientData] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::ConstantsData] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::MeshData] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::TextureData] +
                ResourceAllocator::MEMORY_BUDGET[ResourceCategory::PersistentData],
                HeapType::Default
            })
    {}

    ByteAddressBuffer HeapWithAllocator::createByteAddressBuffer(const ResourceDescription& createDescription)
    {
        Buffer::Description description = createDescription.resource;
        description.dimension = GPUResourceDimension::Buffer;
        ByteAddressBuffer buffer(description);
        buffer.createPlaced(
            mDevice,
            getBackingHeap(createDescription.category),
            mAllocator.allocate(createDescription.category, buffer.getAllocationInfo(mDevice).sizeInBytes)
        );
        return buffer;
    }

    StructuredBuffer HeapWithAllocator::createStructuredBuffer(const ResourceDescription& createDescription, const unsigned int elementCount, const unsigned int strideInBytes)
    {
        Buffer::Description description = createDescription.resource;
        description.dimension = GPUResourceDimension::Buffer;
        StructuredBuffer buffer(description, elementCount, strideInBytes);
        buffer.createPlaced(
            mDevice,
            getBackingHeap(createDescription.category),
            mAllocator.allocate(createDescription.category, buffer.getAllocationInfo(mDevice).sizeInBytes)
        );
        return buffer;
    }

    TypedBuffer HeapWithAllocator::createTypedBuffer(const ResourceDescription& createDescription, const unsigned int elementCount, const unsigned int strideInBytes)
    {
        Buffer::Description description = createDescription.resource;
        description.dimension = GPUResourceDimension::Buffer;
        TypedBuffer buffer(description, elementCount, strideInBytes);
        buffer.createPlaced(
            mDevice,
            getBackingHeap(createDescription.category),
            mAllocator.allocate(createDescription.category, buffer.getAllocationInfo(mDevice).sizeInBytes)
        );
        return buffer;
    }

    Texture HeapWithAllocator::createTexture(const ResourceDescription& createDescription)
    {
        Texture texture(createDescription.resource);
        texture.createPlaced(
            mDevice,
            getBackingHeap(createDescription.category),
            mAllocator.allocate(createDescription.category, texture.getAllocationInfo(mDevice).sizeInBytes)
        );
        return texture;
    }

    RenderTarget HeapWithAllocator::createRenderTarget(Texture& texture, const RenderTarget::Type type)
    {
        return RenderTarget(type, texture);
    }

    void HeapWithAllocator::deallocateBuffer(const ByteAddressBuffer& buffer, const ResourceCategory category)
    {
        mAllocator.deallocate(category, buffer.getHeapOffset(), buffer.getAllocationInfo(mDevice).sizeInBytes);
    }

    void HeapWithAllocator::deallocateBuffer(const StructuredBuffer& buffer, const ResourceCategory category)
    {
        mAllocator.deallocate(category, buffer.getHeapOffset(), buffer.getAllocationInfo(mDevice).sizeInBytes);
    }

    void HeapWithAllocator::deallocateBuffer(const TypedBuffer& buffer, const ResourceCategory category)
    {
        mAllocator.deallocate(category, buffer.getHeapOffset(), buffer.getAllocationInfo(mDevice).sizeInBytes);
    }

    void HeapWithAllocator::deallocateTexture(const Texture& texture, const ResourceCategory category)
    {
        mAllocator.deallocate(category, texture.getHeapOffset(), texture.getAllocationInfo(mDevice).sizeInBytes);
    }
}
