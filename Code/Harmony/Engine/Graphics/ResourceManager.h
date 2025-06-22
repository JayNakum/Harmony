#pragma once

#include "common.h"
#include "Log.h"

#include "dx12/Heap.h"

#include "memory/RingAllocator.h"
#include "memory/BlockAllocator.h"
#include "memory/SuballocationAllocator.h"
#include "memory/LinearAllocator.h"

namespace engine
{
    class ResourceManager
    {
    private:
        class GPUResourceAllocator
        {
        public:
            enum ResourceType
            {
                Upload,
                Transient,
                ConstantBuffer,
                Mesh,
                Texture,
                Persistent,

                COUNT
            };

        private:
            static constexpr size_t MEMORY_BUDGET[ResourceType::COUNT] = {
                memory::MB(128), // Section::Upload
                memory::MB(64), // Section::ConstantBuffer
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
            GPUResourceAllocator()
                : mSections(memory::MB(1664)),
                mUploadData(MEMORY_BUDGET[ResourceType::Upload]),
                mConstantBuffers(MEMORY_BUDGET[ResourceType::ConstantBuffer], 256, mSections.allocate(MEMORY_BUDGET[ResourceType::ConstantBuffer])),
                mTransientData(MEMORY_BUDGET[ResourceType::Transient], mSections.allocate(MEMORY_BUDGET[ResourceType::Transient])),
                mMesh(MEMORY_BUDGET[ResourceType::Mesh], 128, mSections.allocate(MEMORY_BUDGET[ResourceType::Mesh])),
                mTexture(MEMORY_BUDGET[ResourceType::Texture], mSections.allocate(MEMORY_BUDGET[ResourceType::Texture])),
                mPersistentData(MEMORY_BUDGET[ResourceType::Persistent], mSections.allocate(MEMORY_BUDGET[ResourceType::Persistent]))
            {}

            inline const size_t allocate(const ResourceType type, const size_t sizeInBytes)
            {
                switch (type)
                {
                default:
                    HASSERT(false, "Invalid resource type %d.", type);
                    break;
                case ResourceType::Upload:
                    return mUploadData.allocate(sizeInBytes);
                case ResourceType::ConstantBuffer:
                    return mConstantBuffers.allocate(sizeInBytes);
                case ResourceType::Transient:
                    mTransientData.allocate(sizeInBytes);
                    break;
                case ResourceType::Mesh:
                    return mMesh.allocate(sizeInBytes);
                case ResourceType::Texture:
                    return mTexture.allocate(sizeInBytes);
                case ResourceType::Persistent:
                    return mPersistentData.allocate(sizeInBytes);
                }
            }

            inline void deallocate(const ResourceType type, const size_t allocationOffset, const size_t sizeInBytes)
            {
                switch (type)
                {
                default:
                    HASSERT(false, "Invalid resource type %d.", type);
                    break;
                case ResourceType::ConstantBuffer:
                    mConstantBuffers.deallocate(allocationOffset, sizeInBytes);
                    break;
                case ResourceType::Mesh:
                    mMesh.deallocate(allocationOffset, sizeInBytes);
                    break;
                case ResourceType::Texture:
                    mTexture.deallocate(allocationOffset, sizeInBytes);
                    break;
                }
            }

            inline void reset(const ResourceType type)
            {
                switch (type)
                {
                default:
                    HASSERT(false, "Invalid resource type %d.", type);
                    break;
                case ResourceType::Upload:
                    mUploadData.reset();
                    break;
                case ResourceType::ConstantBuffer:
                    mConstantBuffers.reset();
                    break;
                case ResourceType::Transient:
                    mTransientData.reset();
                    break;
                case ResourceType::Mesh:
                    mMesh.reset();
                    break;
                case ResourceType::Texture:
                    mTexture.reset();
                    break;
                case ResourceType::Persistent:
                    mPersistentData.reset();
                    break;
                }
            }

            ~GPUResourceAllocator() = default;

        private:
            memory::LinearAllocator mSections;

            memory::RingAllocator mUploadData;

            memory::BlockAllocator mConstantBuffers;
            memory::RingAllocator mTransientData;
            memory::BlockAllocator mMesh;
            memory::SuballocationAllocator mTexture;
            memory::LinearAllocator mPersistentData;
        };

    public:
        void read();
        void write();

        void create();

    private:
        GPUResourceAllocator mResourceAllocator;

        graphics::dx12::Heap mUploadHeap;
        graphics::dx12::Heap mDefaultHeap;
    };
}
