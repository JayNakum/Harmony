#include "ResourceManager.h"

namespace engine
{
    ResourceManager::ResourceManager(graphics::Device& device)
        : mDevice(device), mResourceHeap(device), mDescriptorHeap(device)
    {}

    void ResourceManager::createBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription)
    {
        HASSERT(mBuffers.find(createDescription.resource.name) == mBuffers.end(), "Buffer resource with ID %s already exists.", createDescription.resource.name.c_str());
        mBuffers.emplace(createDescription.resource.name, mResourceHeap.createBuffer(createDescription));
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

    void ResourceManager::releaseBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceCategory category)
    {
        mResourceHeap.deallocateBuffer(getBuffer(bufferID), category);
        mBuffers.erase(bufferID);
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

    const graphics::DescriptorHandle& ResourceManager::getBufferCBV(const std::string id, std::string alias)
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
        getBuffer(id).createConstantBufferView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getBufferCBV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes, std::string alias)
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
        getBuffer(id).createConstantBufferView(mDevice, mDescriptors.at(alias), sizeInBytes, offsetInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getBufferSRV(const std::string id, std::string alias)
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
        getBuffer(id).createShaderResourceView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getBufferSRV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes, std::string alias)
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
        getBuffer(id).createShaderResourceView(mDevice, mDescriptors.at(alias), sizeInBytes, offsetInBytes);
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getBufferUAV(const std::string id, std::string alias)
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
        getBuffer(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias));
        return mDescriptors.at(alias);
    }

    const graphics::DescriptorHandle& ResourceManager::getBufferUAV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes, std::string alias)
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
        getBuffer(id).createUnorderedAccessView(mDevice, mDescriptors.at(alias), sizeInBytes, offsetInBytes);
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
