#pragma once

#include "common.h"
#include "Log.h"

#include "dx12/Device.h"

#include "dx12/Heap.h"
#include "dx12/GPUResource.h"
#include "dx12/Buffer.h"
#include "dx12/Texture.h"

#include "memory/RingAllocator.h"
#include "memory/BlockAllocator.h"
#include "memory/SuballocationAllocator.h"
#include "memory/LinearAllocator.h"

namespace graphics
{
    class HeapWithAllocator
    {
    public:
        enum ResourceCategory
        {
            Upload,
            UploadPersistent,
            Transient,
            Constants,
            Mesh,
            RenderTarget,
            Texture,
            Persistent,

            COUNT
        };

        struct ResourceDescription
        {
            ResourceCategory category;
            dx12::GPUResource::Description resource;
        };

    public:
        HeapWithAllocator(dx12::Device& device);

        dx12::ByteAddressBuffer createBuffer(const ResourceDescription& createDescription);
        dx12::Texture createTexture(const ResourceDescription& createDescription);
        dx12::RenderTarget createRenderTarget(dx12::Texture& texture, const dx12::RenderTarget::Type type = dx12::RenderTarget::Type::Color);

        void deallocateBuffer(const dx12::ByteAddressBuffer& buffer, const ResourceCategory category);
        void deallocateTexture(const dx12::Texture& texture, const ResourceCategory category);

        inline void reset(const ResourceCategory category) { mAllocator.reset(category); }

        ~HeapWithAllocator() = default;

    private:
        dx12::Device& mDevice;

    private:
        class ResourceAllocator
        {
        public:
            static constexpr size_t MEMORY_BUDGET[ResourceCategory::COUNT] = {
                memory::MB(64), // Category::Upload
                memory::MB(64), // Category::UploadPersistent
                memory::MB(64), // Category::Constants
                memory::MB(64), // Category::Transient
                memory::MB(256), // Category::Mesh
                memory::MB(128), // Category::RenderTarget
                memory::MB(640), // Category::Texture
                memory::MB(256) // Category::Persistent
            };
            // 128 MB for upload heap.
            // 1664 MB for default heap.
            // 256 MB extra.
            // 2 GB TOTAL

        public:
            ResourceAllocator()
                :
                mUploadHeapSections(memory::MB(128)),
                mUploadData(MEMORY_BUDGET[ResourceCategory::Upload], mUploadHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::Upload])),
                mUploadPersistentData(MEMORY_BUDGET[ResourceCategory::UploadPersistent], mUploadHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::UploadPersistent])),
                mDefaultHeapSections(memory::MB(1664)),
                mConstants(MEMORY_BUDGET[ResourceCategory::Constants], 256, mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::Constants])),
                mTransientData(MEMORY_BUDGET[ResourceCategory::Transient], mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::Transient])),
                mMesh(MEMORY_BUDGET[ResourceCategory::Mesh], 128, mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::Mesh])),
                mRenderTarget(MEMORY_BUDGET[ResourceCategory::RenderTarget], mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::RenderTarget])),
                mTexture(MEMORY_BUDGET[ResourceCategory::Texture], mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::Texture])),
                mPersistentData(MEMORY_BUDGET[ResourceCategory::Persistent], mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::Persistent]))
            {}

            inline const uint64_t allocate(const ResourceCategory category, const size_t sizeInBytes)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[category]);

                switch (category)
                {
                default:
                    HASSERT(false, "Invalid resource category %d.", category);
                    break;
                case ResourceCategory::Upload:
                    return mUploadData.allocate(sizeInBytes);
                case ResourceCategory::UploadPersistent:
                    return mUploadPersistentData.allocate(sizeInBytes);
                case ResourceCategory::Constants:
                    return mConstants.allocate(sizeInBytes);
                case ResourceCategory::Transient:
                    return mTransientData.allocate(sizeInBytes);
                case ResourceCategory::Mesh:
                    return mMesh.allocate(sizeInBytes);
                case ResourceCategory::RenderTarget:
                    return mRenderTarget.allocate(sizeInBytes);
                case ResourceCategory::Texture:
                    return mTexture.allocate(sizeInBytes);
                case ResourceCategory::Persistent:
                    return mPersistentData.allocate(sizeInBytes);
                }
            }

            inline void deallocate(const ResourceCategory category, const uint64_t allocationOffset, const size_t sizeInBytes)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[category]);

                switch (category)
                {
                default:
                    //HASSERT_SOFT_ONCE(false, "Invalid resource category %d. No deallocation will happen.", category);
                    break;
                case ResourceCategory::UploadPersistent:
                    mUploadPersistentData.deallocate(allocationOffset, sizeInBytes);
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

            inline void reset(const ResourceCategory category)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[category]);

                switch (category)
                {
                default:
                    HASSERT(false, "Invalid resource category %d.", category);
                    break;
                case ResourceCategory::Upload:
                    mUploadData.reset();
                    break;
                case ResourceCategory::UploadPersistent:
                    mUploadPersistentData.reset();
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
                case ResourceCategory::RenderTarget:
                    mRenderTarget.reset();
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
            memory::LinearAllocator mUploadHeapSections;
            memory::RingAllocator mUploadData;
            memory::SuballocationAllocator mUploadPersistentData;

            memory::LinearAllocator mDefaultHeapSections;
            memory::BlockAllocator mConstants;
            memory::RingAllocator mTransientData;
            memory::BlockAllocator mMesh;
            memory::RingAllocator mRenderTarget;
            memory::SuballocationAllocator mTexture;
            memory::LinearAllocator mPersistentData;

            std::mutex mMutex[ResourceCategory::COUNT];
        };

        ResourceAllocator mAllocator;

    private:
        dx12::Heap mUploadHeap;
        dx12::Heap mDefaultHeap;
        // dx12::Heap mReadbackHeap;
        inline dx12::Heap& getBackingHeap(const ResourceCategory category)
        {
            switch (category)
            {
            case ResourceCategory::Upload:
                return mUploadHeap;
            default:
                return mDefaultHeap;
            }
        }
    };
}
