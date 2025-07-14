#pragma once

#include "common.h"
#include "Log.h"

#include "HeapWithAllocator.h"
#include "DescriptorHeapWithAllocator.h"
#include "ShaderInput.h"

#include "dx12/Device.h"

namespace engine
{
    class ResourceManager
    {
    public:
        ResourceManager(graphics::dx12::Device& device);

        void createBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription);
        void createTexture(const graphics::HeapWithAllocator::ResourceDescription& createDescription);
        void createRenderTarget(const std::string textureID, const graphics::dx12::RenderTarget::Type type = graphics::dx12::RenderTarget::Type::Color);
        void createRenderTarget(const graphics::HeapWithAllocator::ResourceDescription& createDescription, const graphics::dx12::RenderTarget::Type type = graphics::dx12::RenderTarget::Type::Color);

        void releaseBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceCategory category);
        void releaseTexture(const std::string textureID, const graphics::HeapWithAllocator::ResourceCategory category);

        inline graphics::dx12::ByteAddressBuffer& getBuffer(const std::string id) { return mBuffers.at(id); }
        inline graphics::dx12::Texture& getTexture(const std::string id) { return mTextures.at(id); }
        inline graphics::dx12::RenderTarget& getRenderTarget(const std::string id) { return mRenderTargets.at(id); }

        graphics::ResourceTable createResourceTable(const graphics::ResourceTable::Layout layout, const graphics::DescriptorHeapWithAllocator::Category category = graphics::DescriptorHeapWithAllocator::Category::DynamicCPU);

        inline const graphics::dx12::DescriptorHeap& getDescriptorHeap(const graphics::DescriptorHeapWithAllocator::Category category)
        {
            return mDescriptorHeap.getDescriptorHeap(category);
        }

        inline const graphics::dx12::DescriptorHandle allocateDescriptors(const size_t count = 1, const graphics::DescriptorHeapWithAllocator::Category category = graphics::DescriptorHeapWithAllocator::Category::DynamicCPU)
        {
            return mDescriptorHeap.allocate(category, count);
        }

    private:
        graphics::HeapWithAllocator mResourceHeap;
        graphics::DescriptorHeapWithAllocator mDescriptorHeap;

        std::unordered_map<std::string, graphics::dx12::ByteAddressBuffer> mBuffers;
        std::unordered_map<std::string, graphics::dx12::Texture> mTextures;
        std::unordered_map<std::string, graphics::dx12::RenderTarget> mRenderTargets;
    };
}
