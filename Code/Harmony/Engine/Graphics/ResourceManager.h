#pragma once

#include "common.h"

#include "HeapWithAllocator.h"
#include "DescriptorHeapWithAllocator.h"
#include "ShaderInput.h"

#include "dx12/Device.h"

namespace engine
{
    /*
    * Graphics Resource Manager manages the complete lifecycle of any resource on VRAM.
    * It allocates resources inside HeapWithAllocator and DescriptorHeapWithAllocator
    * And maintains a hash map for quick access to all the resources via ID.
    * The user is responsible for ensuring that the ID is unique.
    */
    class ResourceManager
    {
    public:
        ResourceManager(graphics::Device& device);

        // Creates a resource based on the ResourceDescription.
        // The Description::Name is used as ID and has to be unique.
        void createByteAddressBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription);
        void createStructuredBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription, const unsigned int elementCount, const unsigned int strideInBytes);
        void createTypedBuffer(const graphics::HeapWithAllocator::ResourceDescription& createDescription, const unsigned int elementCount, const unsigned int strideInBytes);

        // Creates a resource based on the ResourceDescription.
        // The Description::Name is used as ID and has to be unique.
        void createTexture(const graphics::HeapWithAllocator::ResourceDescription& createDescription);

        // Creates a resource based on the ResourceDescription.
        // The Description::Name is used as ID and has to be unique.
        void createRenderTarget(const std::string textureID, const graphics::RenderTarget::Type type = graphics::RenderTarget::Type::Color);
        void createRenderTarget(const graphics::HeapWithAllocator::ResourceDescription& createDescription, const graphics::RenderTarget::Type type = graphics::RenderTarget::Type::Color);

        // Releases the resource with the specified ID.
        void releaseByteAddressBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceCategory category);
        void releaseStructuredBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceCategory category);
        void releaseTypedBuffer(const std::string bufferID, const graphics::HeapWithAllocator::ResourceCategory category);

        // Releases the resource with the specified ID.
        void releaseTexture(const std::string textureID, const graphics::HeapWithAllocator::ResourceCategory category);

        // Releases the resource with the specified ID.
        void releaseRenderTarget(const std::string textureID, const graphics::HeapWithAllocator::ResourceCategory category);

        // Reset a section of Resource Heap
        inline void reset(const graphics::HeapWithAllocator::ResourceCategory category) { mResourceHeap.reset(category); }
        // Reset a section of Descriptor Heap
        inline void reset(const graphics::DescriptorHeapWithAllocator::DescriptorsCategory category) { mDescriptorHeap.reset(category); }

        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getByteAddressBufferCBV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getByteAddressBufferCBV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes = 0, std::string alias = "NONE");
        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getByteAddressBufferSRV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getByteAddressBufferSRV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes = 0, std::string alias = "NONE");
        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getByteAddressBufferUAV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getByteAddressBufferUAV(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes = 0, std::string alias = "NONE");

        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getStructuredBufferCBV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getStructuredBufferCBV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset = 0, std::string alias = "NONE");
        const graphics::DescriptorHandle& getStructuredBufferCBV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset = 0, std::string alias = "NONE");
        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getStructuredBufferSRV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getStructuredBufferSRV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset = 0, std::string alias = "NONE");
        const graphics::DescriptorHandle& getStructuredBufferSRV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset = 0, std::string alias = "NONE");
        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getStructuredBufferUAV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getStructuredBufferUAV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset = 0, std::string alias = "NONE");
        const graphics::DescriptorHandle& getStructuredBufferUAV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset = 0, std::string alias = "NONE");

        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getTypedBufferCBV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getTypedBufferCBV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset = 0, std::string alias = "NONE");
        const graphics::DescriptorHandle& getTypedBufferCBV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset = 0, std::string alias = "NONE");
        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getTypedBufferSRV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getTypedBufferSRV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset = 0, std::string alias = "NONE");
        const graphics::DescriptorHandle& getTypedBufferSRV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset = 0, std::string alias = "NONE");
        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getTypedBufferUAV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getTypedBufferUAV(const std::string id, const unsigned int elementCount, const unsigned int elementOffset = 0, std::string alias = "NONE");
        const graphics::DescriptorHandle& getTypedBufferUAV(const std::string id, const unsigned int elementCount, const unsigned int strideInBytes, const unsigned int elementOffset = 0, std::string alias = "NONE");

        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getTextureSRV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getTextureSRV(const std::string id, const graphics::Texture::ShaderResourceViewInfo& viewInfo, std::string alias = "NONE");
        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getTextureUAV(const std::string id, std::string alias = "NONE");
        const graphics::DescriptorHandle& getTextureUAV(const std::string id, const graphics::Texture::UnorderedAccessViewInfo& viewInfo, std::string alias = "NONE");
        // Creates or gets a resource view from the mDescriptors hash map.
        // Alias can be used to create a new view with different parameters.
        const graphics::DescriptorHandle& getRenderTargetRTV(const std::string id, std::string alias = "NONE", const uint32_t mipSlice = 0, const uint32_t arraySlice = 0);
        const graphics::DescriptorHandle& getRenderTargetDSV(const std::string id, std::string alias = "NONE", const uint32_t mipSlice = 0, const uint32_t arraySlice = 0, const bool readOnlyDepth = false, const bool readOnlyStencil = false);

        // Destroy a resource view.
        inline void destroyDescriptor(const std::string id) { mDescriptors.erase(id); }

        // Get the graphics::GPUResource Buffer
        inline graphics::ByteAddressBuffer& getByteAddressBuffer(const std::string id) { return mByteAddressBuffers.at(id); }
        inline graphics::StructuredBuffer& getStructuredBuffer(const std::string id) { return mStructuredBuffers.at(id); }
        inline graphics::TypedBuffer& getTypedBuffer(const std::string id) { return mTypedBuffers.at(id); }
        // Get the graphics::GPUResource Texture
        inline graphics::Texture& getTexture(const std::string id) { return mTextures.at(id); }
        // Get the graphics::GPUResource RenderTarget
        inline graphics::RenderTarget& getRenderTarget(const std::string id) { return mRenderTargets.at(id); }

        // Allocates the descriptors for the layout and returns a Resource table.
        // It also maintains a tableID in case of reusing.
        graphics::ResourceTable& createResourceTable(const std::string tableID, const graphics::ResourceTable::Layout layout, const graphics::DescriptorHeapWithAllocator::DescriptorsCategory category = graphics::DescriptorHeapWithAllocator::DescriptorsCategory::DynamicCPU);
        void destroyResourceTable(const std::string tableID);

        inline const graphics::DescriptorHeap& getDescriptorHeap(const graphics::DescriptorHeapWithAllocator::DescriptorsCategory category)
        {
            return mDescriptorHeap.getDescriptorHeap(category);
        }

        // Directly allocate descriptors.
        inline const graphics::DescriptorHandle allocateDescriptors(const size_t count = 1, const graphics::DescriptorHeapWithAllocator::DescriptorsCategory category = graphics::DescriptorHeapWithAllocator::DescriptorsCategory::DynamicCPU)
        {
            return mDescriptorHeap.allocate(category, count);
        }

    private:
        graphics::Device& mDevice;
        graphics::HeapWithAllocator mResourceHeap;
        graphics::DescriptorHeapWithAllocator mDescriptorHeap;

        std::unordered_map<std::string, graphics::ByteAddressBuffer> mByteAddressBuffers;
        std::unordered_map<std::string, graphics::StructuredBuffer> mStructuredBuffers;
        std::unordered_map<std::string, graphics::TypedBuffer> mTypedBuffers;
        std::unordered_map<std::string, graphics::Texture> mTextures;
        std::unordered_map<std::string, graphics::RenderTarget> mRenderTargets;

        std::unordered_map<std::string, graphics::ResourceTable> mResourceTables;
        std::unordered_map<std::string, graphics::DescriptorHandle> mDescriptors;
    };
}
