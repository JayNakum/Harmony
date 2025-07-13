#include "ResourceManager.h"

namespace engine
{
    ResourceManager::ResourceManager(graphics::dx12::Device& device)
        : mResourceHeap(device), mDescriptorHeap(device)
    {}

    void ResourceManager::createBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceDescription & createDescription)
    {
        HASSERT(mBuffers.find(bufferID) == mBuffers.end(), "Buffer resource with ID %s already exists.", bufferID);
        mBuffers.emplace(bufferID, mResourceHeap.createBuffer(createDescription));
    }

    void ResourceManager::createTexture(const std::string textureID, const graphics::HeapWithAllocator::ResourceDescription& createDescription)
    {
        HASSERT(mTextures.find(textureID) == mTextures.end(), "Texture resource with ID %s already exists.", textureID);
        mTextures.emplace(textureID, mResourceHeap.createBuffer(createDescription));
    }

    void ResourceManager::createRenderTarget(const std::string renderTargetID, const std::string textureID, const graphics::dx12::RenderTarget::Type type)
    {
        HASSERT(mRenderTargets.find(renderTargetID) == mRenderTargets.end(), "RenderTarget resource with ID = %s already exists.", renderTargetID);
        mRenderTargets.emplace(renderTargetID, mResourceHeap.createRenderTarget(getTexture(textureID), type));

    }

    void ResourceManager::createRenderTarget(const std::string renderTargetID, const graphics::HeapWithAllocator::ResourceDescription& createDescription, const graphics::dx12::RenderTarget::Type type)
    {
        HASSERT(mRenderTargets.find(renderTargetID) == mRenderTargets.end(), "RenderTarget resource with ID = %s already exists.", renderTargetID);
        mRenderTargets.emplace(renderTargetID, mResourceHeap.createRenderTarget(createDescription, type));
    }

    graphics::ResourceTable ResourceManager::createResourceTable(const graphics::ResourceTable::Layout layout, const graphics::DescriptorHeapWithAllocator::Category category)
    {
        size_t count = 0;
        for (graphics::ResourceTable::Range range : layout.ranges)
        {
            count += range.count;
        }
        return graphics::ResourceTable(layout, mDescriptorHeap.allocate(category, count));
    }
}

