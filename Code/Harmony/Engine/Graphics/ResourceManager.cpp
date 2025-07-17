#include "ResourceManager.h"

namespace engine
{
    ResourceManager::ResourceManager(graphics::dx12::Device& device)
        : mResourceHeap(device), mDescriptorHeap(device)
    {}

    void ResourceManager::createBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription)
    {
        //HASSERT(mBuffers.find(createDescription.resource.name) == mBuffers.end(), "Buffer resource with ID %s already exists.", createDescription.resource.name.c_str());
        mBuffers.emplace(createDescription.resource.name, mResourceHeap.createBuffer(createDescription));
    }

    void ResourceManager::createTexture(const graphics::HeapWithAllocator::ResourceDescription& createDescription)
    {
        //HASSERT(mTextures.find(createDescription.resource.name) == mTextures.end(), "Texture resource with ID %s already exists.", createDescription.resource.name.c_str());
        mTextures.emplace(createDescription.resource.name, mResourceHeap.createTexture(createDescription));
    }

    void ResourceManager::createRenderTarget(const std::string textureID, const graphics::dx12::RenderTarget::Type type)
    {
        //HASSERT(mRenderTargets.find(textureID) == mRenderTargets.end(), "RenderTarget resource with ID = %s already exists.", textureID);
        mRenderTargets.emplace(textureID, mResourceHeap.createRenderTarget(getTexture(textureID), type));
    }

    void ResourceManager::createRenderTarget(const graphics::HeapWithAllocator::ResourceDescription& createDescription, const graphics::dx12::RenderTarget::Type type)
    {
        //HASSERT(mRenderTargets.find(createDescription.resource.name) == mRenderTargets.end(), "RenderTarget resource with ID = %s already exists.", createDescription.resource.name.c_str());

        createTexture(createDescription);
        mRenderTargets.emplace(createDescription.resource.name, mResourceHeap.createRenderTarget(getTexture(createDescription.resource.name), type));
    }

    void ResourceManager::setRenderTarget(const std::string textureID, graphics::dx12::RenderTarget& renderTarget)
    {
        mTextures.emplace(textureID, renderTarget.getTexture());
        mRenderTargets.emplace(textureID, renderTarget);
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

    graphics::ResourceTable& ResourceManager::createResourceTable(const std::string tableID, const graphics::ResourceTable::Layout layout, const graphics::DescriptorHeapWithAllocator::DescriptorsCategory category)
    {
        //HASSERT(mResourceTables.find(tableID) == mResourceTables.end(), "ResourceTable with ID = %s already exists.", tableID.c_str());

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
