#include "ResourceManager.h"

namespace engine
{
    ResourceManager::ResourceManager(graphics::Device& device)
        : mDevice(device), mResourceHeap(device), mDescriptorHeap(device) {
    }

    void ResourceManager::createByteAddressBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription)
    {
        HASSERT(mByteAddressBuffers.find(createDescription.resource.name) == mByteAddressBuffers.end(), "ByteAddressBuffer resource with ID %s already exists.", createDescription.resource.name.c_str());
        mByteAddressBuffers.emplace(createDescription.resource.name, mResourceHeap.createByteAddressBuffer(createDescription));
    }

    void ResourceManager::createStructuredBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription, const unsigned int elementCount, const unsigned int strideInBytes)
    {
        HASSERT(mStructuredBuffers.find(createDescription.resource.name) == mStructuredBuffers.end(), "StructuredBuffers resource with ID %s already exists.", createDescription.resource.name.c_str());
        mStructuredBuffers.emplace(createDescription.resource.name, mResourceHeap.createStructuredBuffer(createDescription, elementCount, strideInBytes));
    }

    void ResourceManager::createTypedBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription, const unsigned int elementCount, const unsigned int strideInBytes)
    {
        HASSERT(mTypedBuffers.find(createDescription.resource.name) == mTypedBuffers.end(), "TypedBuffer resource with ID %s already exists.", createDescription.resource.name.c_str());
        mTypedBuffers.emplace(createDescription.resource.name, mResourceHeap.createTypedBuffer(createDescription, elementCount, strideInBytes));
    }

    void ResourceManager::createTexture(const graphics::HeapWithAllocator::ResourceDescription& createDescription)
    {
        HASSERT(mTextures.find(createDescription.resource.name) == mTextures.end(), "Texture resource with ID %s already exists.", createDescription.resource.name.c_str());
        mTextures.emplace(createDescription.resource.name, mResourceHeap.createTexture(createDescription));
    }

    void ResourceManager::createRenderTarget(const std::string textureID, const graphics::RenderTarget::Type type)
    {
        HASSERT(mRenderTargets.find(textureID) == mRenderTargets.end(), "RenderTarget resource with ID = %s already exists.", textureID.c_str());
        mRenderTargets.emplace(textureID, mResourceHeap.createRenderTarget(getTexture(textureID), type));
    }

    void ResourceManager::createRenderTarget(const graphics::HeapWithAllocator::ResourceDescription& createDescription, const graphics::RenderTarget::Type type)
    {
        //HASSERT(mRenderTargets.find(createDescription.resource.name) == mRenderTargets.end(), "RenderTarget resource with ID = %s already exists.", createDescription.resource.name.c_str());

        createTexture(createDescription);
        mRenderTargets.emplace(createDescription.resource.name, mResourceHeap.createRenderTarget(getTexture(createDescription.resource.name), type));
    }

    void ResourceManager::releaseByteAddressBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceCategory category)
    {
        mResourceHeap.deallocateBuffer(getByteAddressBuffer(bufferID), category);
        mByteAddressBuffers.erase(bufferID);
    }

    void ResourceManager::releaseStructuredBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceCategory category)
    {
        mResourceHeap.deallocateBuffer(getStructuredBuffer(bufferID), category);
        mStructuredBuffers.erase(bufferID);
    }

    void ResourceManager::releaseTypedBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceCategory category)
    {
        mResourceHeap.deallocateBuffer(getTypedBuffer(bufferID), category);
        mTypedBuffers.erase(bufferID);
    }

    void ResourceManager::releaseTexture(const std::string textureID, const graphics::HeapWithAllocator::ResourceCategory category)
    {
        mResourceHeap.deallocateTexture(getTexture(textureID), category);
        mTextures.erase(textureID);
    }

    void ResourceManager::releaseRenderTarget(const std::string textureID, const graphics::HeapWithAllocator::ResourceCategory category)
    {
        releaseTexture(textureID, category);
        mRenderTargets.erase(textureID);
    }

    const graphics::DescriptorHandle& ResourceManager::getByteAddressBufferCBV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getByteAddressBuffer(id).createConstantBufferView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getByteAddressBufferCBV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getByteAddressBuffer(id).createConstantBufferView(mDevice, mDescriptors.at(alias), sizeInBytes, offsetInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getByteAddressBufferSRV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getByteAddressBuffer(id).createShaderResourceView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getByteAddressBufferSRV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getByteAddressBuffer(id).createShaderResourceView(mDevice, mDescriptors.at(alias), sizeInBytes, offsetInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getByteAddressBufferUAV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getByteAddressBuffer(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getByteAddressBufferUAV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getByteAddressBuffer(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias), sizeInBytes, offsetInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferCBV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getStructuredBuffer(id).createConstantBufferView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferCBV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        graphics::StructuredBuffer& structuredBuffer = getStructuredBuffer(id);
        const unsigned int strideInBytes = structuredBuffer.getStrideInBytes();

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        structuredBuffer.createConstantBufferView(mDevice, mDescriptors.at(alias), elementCount * strideInBytes, elementOffset * strideInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferCBV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getStructuredBuffer(id).createConstantBufferView(mDevice, mDescriptors.at(alias), elementCount * strideInBytes, elementOffset * strideInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferSRV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getStructuredBuffer(id).createShaderResourceView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferSRV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        graphics::StructuredBuffer& structuredBuffer = getStructuredBuffer(id);
        const unsigned int strideInBytes = structuredBuffer.getStrideInBytes();

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        structuredBuffer.createShaderResourceView(mDevice, mDescriptors.at(alias), elementCount, strideInBytes, elementOffset);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferSRV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getStructuredBuffer(id).createShaderResourceView(mDevice, mDescriptors.at(alias), elementCount, strideInBytes, elementOffset);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferUAV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getStructuredBuffer(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferUAV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        graphics::StructuredBuffer& structuredBuffer = getStructuredBuffer(id);
        const unsigned int strideInBytes = structuredBuffer.getStrideInBytes();

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        structuredBuffer.createUnorderedAccessView(mDevice, mDescriptors.at(alias), elementCount, strideInBytes, elementOffset);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getStructuredBufferUAV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getStructuredBuffer(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias), elementCount, strideInBytes, elementOffset);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferCBV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getTypedBuffer(id).createConstantBufferView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferCBV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        graphics::TypedBuffer& typedBuffer = getTypedBuffer(id);
        const unsigned int strideInBytes = typedBuffer.getStrideInBytes();

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        typedBuffer.createConstantBufferView(mDevice, mDescriptors.at(alias), elementCount * strideInBytes, elementOffset * strideInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferCBV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getTypedBuffer(id).createConstantBufferView(mDevice, mDescriptors.at(alias), elementCount * strideInBytes, elementOffset * strideInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferSRV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getTypedBuffer(id).createShaderResourceView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferSRV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        graphics::TypedBuffer& typedBuffer = getTypedBuffer(id);
        const unsigned int strideInBytes = typedBuffer.getStrideInBytes();

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        typedBuffer.createShaderResourceView(mDevice, mDescriptors.at(alias), elementCount, strideInBytes, elementOffset);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferSRV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getTypedBuffer(id).createShaderResourceView(mDevice, mDescriptors.at(alias), elementCount, strideInBytes, elementOffset);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferUAV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getTypedBuffer(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferUAV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        graphics::TypedBuffer& typedBuffer = getTypedBuffer(id);
        const unsigned int strideInBytes = typedBuffer.getStrideInBytes();

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        typedBuffer.createUnorderedAccessView(mDevice, mDescriptors.at(alias), elementCount, strideInBytes, elementOffset);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTypedBufferUAV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getTypedBuffer(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias), elementCount, strideInBytes, elementOffset);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTextureSRV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        const graphics::Texture::ShaderResourceViewInfo viewInfo = graphics::Texture::ShaderResourceViewInfo::getSRVInfo(getTexture(id).getDescription());
        getTexture(id).createShaderResourceView(mDevice, mDescriptors.at(alias), viewInfo);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTextureSRV(const std::string id, const graphics::Texture::ShaderResourceViewInfo& viewInfo, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getTexture(id).createShaderResourceView(mDevice, mDescriptors.at(alias), viewInfo);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTextureUAV(const std::string id, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        const graphics::Texture::UnorderedAccessViewInfo viewInfo = graphics::Texture::UnorderedAccessViewInfo::getUAVInfo(getTexture(id).getDescription());
        getTexture(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias), viewInfo);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getTextureUAV(const std::string id, const graphics::Texture::UnorderedAccessViewInfo& viewInfo, std::string alias)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate());
        getTexture(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias), viewInfo);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getRenderTargetRTV(const std::string id, std::string alias, const uint32_t mipSlice, const uint32_t arraySlice)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate(graphics::DescriptorHeapWithAllocator::DescriptorsCategory::RenderTargetCPU));
        getRenderTarget(id).createRenderTargetView(mDevice, mDescriptors.at(alias), mipSlice, arraySlice);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getRenderTargetDSV(const std::string id, std::string alias, const uint32_t mipSlice, const uint32_t arraySlice, const bool readOnlyDepth, const bool readOnlyStencil)
    {
        if (alias == "NONE")
        {
            alias = id;
        }

        if (mDescriptors.find(alias) != mDescriptors.end())
        {
            return mDescriptors[alias];
        }

        mDescriptors.emplace(alias, mDescriptorHeap.allocate(graphics::DescriptorHeapWithAllocator::DescriptorsCategory::DepthStencilCPU));
        getRenderTarget(id).createDepthStencilView(mDevice, mDescriptors.at(alias), mipSlice, arraySlice, readOnlyDepth, readOnlyStencil);
        return mDescriptors.at(alias);
    }

    graphics::ResourceTable& ResourceManager::createResourceTable(const std::string tableID, const graphics::ResourceTable::Layout layout, const graphics::DescriptorHeapWithAllocator::DescriptorsCategory category)
    {
        if (mResourceTables.find(tableID) != mResourceTables.end())
        {
            return mResourceTables.at(tableID);
        }

        size_t count = 0;
        for (graphics::ResourceTable::Range range : layout.ranges)
        {
            count += range.count;
        }

        mResourceTables.emplace(tableID, graphics::ResourceTable(layout, mDescriptorHeap.allocate(category, count)));

        return mResourceTables.at(tableID);
    }

    void ResourceManager::destroyResourceTable(const std::string tableID)
    {
        mResourceTables.erase(tableID);
    }
}
