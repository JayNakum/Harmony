#pragma once

#include "common.h"

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
    /*
    * A resource heap with a custom allocator.
    * It allocates the resource based on the specified ResourceCategory.
    * It contains all different type of heaps ie Upload, Default and ReadBack.
    */
    class HeapWithAllocator
    {
    public:
        enum ResourceCategory
        {
            UploadData,
            UploadPersistentData,
            TransientData,
            ConstantsData,
            MeshData,
            RenderTargetData,
            TextureData,
            PersistentData,

            COUNT
        };

        struct ResourceDescription
        {
            ResourceCategory category;
            GPUResource::Description resource;
        };

    public:
        HeapWithAllocator(Device& device);

        ByteAddressBuffer createByteAddressBuffer(const ResourceDescription& createDescription);
        StructuredBuffer createStructuredBuffer(const ResourceDescription& createDescription, const unsigned int elementCount, const unsigned int strideInBytes);
        TypedBuffer createTypedBuffer(const ResourceDescription& createDescription, const unsigned int elementCount, const unsigned int strideInBytes);
        Texture createTexture(const ResourceDescription& createDescription);
        RenderTarget createRenderTarget(Texture& texture, const RenderTarget::Type type = RenderTarget::Type::Color);

        void deallocateBuffer(const ByteAddressBuffer& buffer, const ResourceCategory category);
        void deallocateBuffer(const StructuredBuffer& buffer, const ResourceCategory category);
        void deallocateBuffer(const TypedBuffer& buffer, const ResourceCategory category);
        void deallocateTexture(const Texture& texture, const ResourceCategory category);

        inline void reset(const ResourceCategory category) { mAllocator.reset(category); }

        ~HeapWithAllocator() = default;

    private:
        Device& mDevice;

    private:
        /*
        * Custom allocator using the basic allocators from Core/Memory
        * Linear allocator is used to virtually segment the heap and a different allocator is used based on the resource category.
        */
        class ResourceAllocator
        {
        public:
            // Total memory budget of GPU memory.
            // This includes the budget of Upload heap and Readback heap as well.
            // 2 GB TOTAL.
            // 128 MB for upload heap.
            // 1664 MB for default heap.
            // 256 MB extra.
            static constexpr std::array<size_t, ResourceCategory::COUNT> MEMORY_BUDGET = {
                memory::MB(64), // Category::Upload
                memory::MB(64), // Category::UploadPersistent
                memory::MB(64), // Category::Constants
                memory::MB(64), // Category::Transient
                memory::MB(256), // Category::Mesh
                memory::MB(128), // Category::RenderTarget
                memory::MB(640), // Category::Texture
                memory::MB(256) // Category::Persistent
            };

        public:
            ResourceAllocator()
                :
                mUploadHeapSections(memory::MB(128)),
                mUploadData(MEMORY_BUDGET[ResourceCategory::UploadData], mUploadHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::UploadData])),
                mUploadPersistentData(MEMORY_BUDGET[ResourceCategory::UploadPersistentData], mUploadHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::UploadPersistentData])),
                mDefaultHeapSections(memory::MB(1664)),
                mConstants(MEMORY_BUDGET[ResourceCategory::ConstantsData], 256, mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::ConstantsData])),
                mTransientData(MEMORY_BUDGET[ResourceCategory::TransientData], mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::TransientData])),
                mMesh(MEMORY_BUDGET[ResourceCategory::MeshData], 128, mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::MeshData])),
                mRenderTarget(MEMORY_BUDGET[ResourceCategory::RenderTargetData], mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::RenderTargetData])),
                mTexture(MEMORY_BUDGET[ResourceCategory::TextureData], mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::TextureData])),
                mPersistentData(MEMORY_BUDGET[ResourceCategory::PersistentData], mDefaultHeapSections.allocate(MEMORY_BUDGET[ResourceCategory::PersistentData]))
            {
            }

            // Allocates the sizeInBytes and returns an allocation offset for the specified category.
            inline uint64_t allocate(const ResourceCategory category, const size_t sizeInBytes)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[category]);

                switch (category)
                {
                default:
                case ResourceCategory::COUNT:
                    HASSERT(false, "Invalid resource category ", category);
                    break;
                case ResourceCategory::UploadData:
                    return mUploadData.allocate(sizeInBytes);
                case ResourceCategory::UploadPersistentData:
                    return mUploadPersistentData.allocate(sizeInBytes);
                case ResourceCategory::ConstantsData:
                    return mConstants.allocate(sizeInBytes);
                case ResourceCategory::TransientData:
                    return mTransientData.allocate(sizeInBytes);
                case ResourceCategory::MeshData:
                    return mMesh.allocate(sizeInBytes);
                case ResourceCategory::RenderTargetData:
                    return mRenderTarget.allocate(sizeInBytes);
                case ResourceCategory::TextureData:
                    return mTexture.allocate(sizeInBytes);
                case ResourceCategory::PersistentData:
                    return mPersistentData.allocate(sizeInBytes);
                }
            }

            // Provides a method to deallocate a specific offset but since ring allocators reuse the memory, no deallocation happens for ring.
            inline void deallocate(const ResourceCategory category, const uint64_t allocationOffset, const size_t sizeInBytes)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[category]);

                switch (category)
                {
                default:
                case ResourceCategory::COUNT:
                case ResourceCategory::UploadData:
                case ResourceCategory::RenderTargetData:
                case ResourceCategory::PersistentData:
                case ResourceCategory::TransientData:
                    //HASSERT_SOFT_ONCE(false, "Invalid resource category %d. No deallocation will happen.", category);
                    break;
                case ResourceCategory::UploadPersistentData:
                    mUploadPersistentData.deallocate(allocationOffset, sizeInBytes);
                    break;
                case ResourceCategory::ConstantsData:
                    mConstants.deallocate(allocationOffset, sizeInBytes);
                    break;
                case ResourceCategory::MeshData:
                    mMesh.deallocate(allocationOffset, sizeInBytes);
                    break;
                case ResourceCategory::TextureData:
                    mTexture.deallocate(allocationOffset, sizeInBytes);
                    break;
                }
            }

            // Resets the allocator for the specified ResourceCategory.
            inline void reset(const ResourceCategory category)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[category]);

                switch (category)
                {
                default:
                case ResourceCategory::COUNT:
                    HASSERT(false, "Invalid resource category ", category);
                    break;
                case ResourceCategory::UploadData:
                    mUploadData.reset();
                    break;
                case ResourceCategory::UploadPersistentData:
                    mUploadPersistentData.reset();
                    break;
                case ResourceCategory::ConstantsData:
                    mConstants.reset();
                    break;
                case ResourceCategory::TransientData:
                    mTransientData.reset();
                    break;
                case ResourceCategory::MeshData:
                    mMesh.reset();
                    break;
                case ResourceCategory::RenderTargetData:
                    mRenderTarget.reset();
                    break;
                case ResourceCategory::TextureData:
                    mTexture.reset();
                    break;
                case ResourceCategory::PersistentData:
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

            std::array<std::mutex, ResourceCategory::COUNT> mMutex;
        };

        ResourceAllocator mAllocator;

    private:
        Heap mUploadHeap;
        Heap mDefaultHeap;
        // Heap mReadbackHeap;

        inline Heap& getBackingHeap(const ResourceCategory category)
        {
            switch (category)
            {
            case ResourceCategory::UploadData:
            case ResourceCategory::UploadPersistentData:
                return mUploadHeap;

            case ResourceCategory::TransientData:
            case ResourceCategory::ConstantsData:
            case ResourceCategory::MeshData:
            case ResourceCategory::RenderTargetData:
            case ResourceCategory::TextureData:
            case ResourceCategory::PersistentData:
            case ResourceCategory::COUNT:
            default:
                return mDefaultHeap;
            }
        }
    };
}
