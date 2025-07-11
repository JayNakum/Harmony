#pragma once

#include "common.h"
#include "Log.h"

#include "dx12/Heap.h"

#include "dx12/GPUResource.h"
#include "dx12/Buffer.h"
#include "dx12/Texture.h"

#include "memory/RingAllocator.h"
#include "memory/BlockAllocator.h"
#include "memory/SuballocationAllocator.h"
#include "memory/LinearAllocator.h"

namespace engine
{
    class ResourceManager
    {
    public:
        enum ResourceCategory
        {
            Upload,
            Transient,
            Constants,
            Mesh,
            Texture,
            Persistent,

            COUNT
        };

        struct ResourceDescription
        {
            std::string id;
            union
            {
                struct BufferDescription
                {
                    size_t size;
                }buffer;

                struct TextureDescription
                {
                    size_t width;
                    size_t height;
                    graphics::Format format;
                }texture;
            };
        };

    public:
        void createConstantBuffer(const ResourceDescription& description);
        void writeConstantBuffer(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes);
        void readConstantBuffer(const std::string id, const uint64_t sizeInBytes, const unsigned int offsetInBytes);

        void createTexture(const ResourceDescription& description);
        void writeTexture(const std::string id, const graphics::dx12::Texture::ShaderResourceViewInfo& viewInfo);
        void readTexture(const std::string id, const graphics::dx12::Texture::ShaderResourceViewInfo& viewInfo);

        void createRenderTarget(const ResourceDescription& description);
        void writeRenderTarget(const std::string id, const graphics::dx12::Texture::ShaderResourceViewInfo& viewInfo);
        void readRenderTarget(const std::string id, const graphics::dx12::Texture::ShaderResourceViewInfo& viewInfo);

    private:
        class ResourceAllocator
        {
        private:
            static constexpr size_t MEMORY_BUDGET[ResourceCategory::COUNT] = {
                memory::MB(128), // Section::Upload
                memory::MB(64), // Section::Constants
                memory::MB(64), // Section::Transient
                memory::MB(256), // Section::Mesh
                memory::GB(1), // Section::Texture
                memory::MB(256) // Section::Persistent
            };
            // 128 MB for upload heap.
            // 1664 MB for default heap.
            // 256 MB extra.
            // 2 GB TOTAL

        public:
            ResourceAllocator()
                : mSections(memory::MB(1664)),
                mUploadData(MEMORY_BUDGET[ResourceCategory::Upload]),
                mConstants(MEMORY_BUDGET[ResourceCategory::Constants], 256, mSections.allocate(MEMORY_BUDGET[ResourceCategory::Constants])),
                mTransientData(MEMORY_BUDGET[ResourceCategory::Transient], mSections.allocate(MEMORY_BUDGET[ResourceCategory::Transient])),
                mMesh(MEMORY_BUDGET[ResourceCategory::Mesh], 128, mSections.allocate(MEMORY_BUDGET[ResourceCategory::Mesh])),
                mTexture(MEMORY_BUDGET[ResourceCategory::Texture], mSections.allocate(MEMORY_BUDGET[ResourceCategory::Texture])),
                mPersistentData(MEMORY_BUDGET[ResourceCategory::Persistent], mSections.allocate(MEMORY_BUDGET[ResourceCategory::Persistent]))
            {}

            inline const size_t allocate(const ResourceCategory type, const size_t sizeInBytes)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[type]);

                switch (type)
                {
                default:
                    HASSERT(false, "Invalid resource type %d.", type);
                    break;
                case ResourceCategory::Upload:
                    return mUploadData.allocate(sizeInBytes);
                case ResourceCategory::Constants:
                    return mConstants.allocate(sizeInBytes);
                case ResourceCategory::Transient:
                    return mTransientData.allocate(sizeInBytes);
                case ResourceCategory::Mesh:
                    return mMesh.allocate(sizeInBytes);
                case ResourceCategory::Texture:
                    return mTexture.allocate(sizeInBytes);
                case ResourceCategory::Persistent:
                    return mPersistentData.allocate(sizeInBytes);
                }
            }

            inline void deallocate(const ResourceCategory type, const size_t allocationOffset, const size_t sizeInBytes)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[type]);

                switch (type)
                {
                default:
                    HASSERT(false, "Invalid resource type %d.", type);
                    break;
                case ResourceCategory::Constants:
                    mConstants.deallocate(allocationOffset, sizeInBytes);
                    break;
                case ResourceCategory::Mesh:
                    mMesh.deallocate(allocationOffset, sizeInBytes);
                    break;
                case ResourceCategory::Texture:
                    mTexture.deallocate(allocationOffset, sizeInBytes);
                    break;
                }
            }

            inline void reset(const ResourceCategory type)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[type]);

                switch (type)
                {
                default:
                    HASSERT(false, "Invalid resource type %d.", type);
                    break;
                case ResourceCategory::Upload:
                    mUploadData.reset();
                    break;
                case ResourceCategory::Constants:
                    mConstants.reset();
                    break;
                case ResourceCategory::Transient:
                    mTransientData.reset();
                    break;
                case ResourceCategory::Mesh:
                    mMesh.reset();
                    break;
                case ResourceCategory::Texture:
                    mTexture.reset();
                    break;
                case ResourceCategory::Persistent:
                    mPersistentData.reset();
                    break;
                }
            }

            ~ResourceAllocator() = default;

        private:
            memory::LinearAllocator mSections;

            memory::RingAllocator mUploadData;
            memory::BlockAllocator mConstants;
            memory::RingAllocator mTransientData;
            memory::BlockAllocator mMesh;
            memory::SuballocationAllocator mTexture;
            memory::LinearAllocator mPersistentData;

            std::mutex mMutex[ResourceCategory::COUNT];
        };

    private:
        ResourceAllocator mResourceAllocator;

        graphics::dx12::Heap mUploadHeap;
        graphics::dx12::Heap mDefaultHeap;
        // graphics::dx12::Heap mReadbackHeap;
    };
}
