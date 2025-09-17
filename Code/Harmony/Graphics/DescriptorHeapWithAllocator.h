#pragma once

#include "common.h"
#include "Log.h"

#include "dx12/Device.h"

#include "dx12/DescriptorHeap.h"

#include "memory/RingAllocator.h"
#include "memory/LinearAllocator.h"

namespace graphics
{
    /*
    * A descriptor heap with a custom allocator.
    * It allocates the descriptors based on the specified DescriptorCategory.
    * It contains CBV_SRV_UAV CPU, CBV_SRV_UAV GPU and RTV CPU and DSV CPU heaps.
    * The class also allows batch copy of descriptors.
    */
    class DescriptorHeapWithAllocator
    {
    public:
        enum DescriptorsCategory
        {
            StaticGPU,
            DynamicCPU,
            DynamicGPU,
            RenderTargetCPU,
            DepthStencilCPU,

            COUNT
        };

    public:
        DescriptorHeapWithAllocator(Device& device);

        DescriptorHandle allocate(const DescriptorsCategory category = DescriptorsCategory::DynamicCPU, const size_t count = 1);

        // Batched copy of descriptors into this heap from one or more source ranges.
        void copyDescriptorsFrom(Device& device, const DescriptorHandle& destStart,
                                 const std::vector<DescriptorHandle>& srcStarts,
                                 const std::vector<unsigned int>& srcCounts,
                                 const DescriptorsCategory category = DescriptorsCategory::DynamicCPU);

        inline void reset(const DescriptorsCategory category) { mAllocator.reset(category); }

        inline DescriptorHeap& getDescriptorHeap(const DescriptorsCategory category)
        {
            switch (category)
            {
            default:
                HASSERT_SOFT(false, "Invalid category(%d) for descriptor heap. Using ShaderViewsDescriptorHeap.", category);
            case DescriptorsCategory::DynamicCPU:
                return mShaderViewsDescriptorHeapCPU;
            case DescriptorsCategory::StaticGPU:
            case DescriptorsCategory::DynamicGPU:
                return mShaderViewsDescriptorHeapGPU;
            case DescriptorsCategory::RenderTargetCPU:
                return mRenderTargetViewsDescriptorHeap;
            case DescriptorsCategory::DepthStencilCPU:
                return mDepthStencilViewsDescriptorHeap;
            }
        }

    private:
        Device& mDevice;

    private:
        /*
        * Custom allocator using the basic allocators from Core/Memory
        * Linear allocator is used to virtually segment the heap and ring allocator is used for each category.
        */
        class DescriptorAllocator
        {
        public:
            // Total memory budget of GPU memory.
            // This includes the budget of Upload heap and Readback heap as well.
            // 5 MB TOTAL.
            // 2 MB for GPU heap (StaticGPU + DynamicGPU).
            // 1 MB for CPU heap.
            // 1 MB for RTV CPU heap.
            // 1 MB for DSV CPU heap.
            static constexpr std::array<size_t, DescriptorsCategory::COUNT>MEMORY_BUDGET = {
                memory::MB(1), // DescriptorsCategory::StaticGPU
                memory::MB(1), // DescriptorsCategory::DynamicCPU
                memory::MB(1), // DescriptorsCategory::DynamicGPU
                memory::MB(1), // DescriptorsCategory::RenderTargetCPU
                memory::MB(1) // DescriptorsCategory::DepthStencilCPU
            };

            DescriptorAllocator()
                : mSections(memory::MB(3)),
                mStaticDescriptors(MEMORY_BUDGET[DescriptorsCategory::StaticGPU], mSections.allocate(MEMORY_BUDGET[DescriptorsCategory::StaticGPU])),
                mDynamicDescriptorsGPU(MEMORY_BUDGET[DescriptorsCategory::DynamicGPU], mSections.allocate(MEMORY_BUDGET[DescriptorsCategory::DynamicGPU])),
                mDynamicDescriptorsCPU(MEMORY_BUDGET[DescriptorsCategory::DynamicCPU]),
                mRenderTargetDescriptors(MEMORY_BUDGET[DescriptorsCategory::RenderTargetCPU]),
                mDepthStencilDescriptors(MEMORY_BUDGET[DescriptorsCategory::DepthStencilCPU])
            {}

            // Allocates the sizeInBytes and returns an allocation offset for the specified category.
            inline const uint64_t allocate(const DescriptorsCategory section, const size_t sizeInBytes)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[section]);

                switch (section)
                {
                default:
                    HASSERT_SOFT(false, "Invalid section(%d) for descriptor allocation. Using DynamicCPU section.", section);
                case DescriptorsCategory::DynamicCPU:
                    return mDynamicDescriptorsCPU.allocate(sizeInBytes);
                case DescriptorsCategory::StaticGPU:
                    return mStaticDescriptors.allocate(sizeInBytes);
                case DescriptorsCategory::DynamicGPU:
                    return mDynamicDescriptorsGPU.allocate(sizeInBytes);
                case DescriptorsCategory::RenderTargetCPU:
                    return mRenderTargetDescriptors.allocate(sizeInBytes);
                case DescriptorsCategory::DepthStencilCPU:
                    return mDepthStencilDescriptors.allocate(sizeInBytes);
                }
            }

            // Resets the allocator for the specified ResourceCategory.
            inline void reset(const DescriptorsCategory section)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[section]);

                switch (section)
                {
                default:
                    HASSERT(false, "Invalid section(%d) for descriptor allocation.", section);
                case DescriptorsCategory::DynamicCPU:
                    mDynamicDescriptorsCPU.reset();
                    break;
                case DescriptorsCategory::StaticGPU:
                    mStaticDescriptors.reset();
                    break;
                case DescriptorsCategory::DynamicGPU:
                    mDynamicDescriptorsGPU.reset();
                    break;
                case DescriptorsCategory::RenderTargetCPU:
                    mRenderTargetDescriptors.reset();
                    break;
                case DescriptorsCategory::DepthStencilCPU:
                    mDepthStencilDescriptors.reset();
                    break;
                }
            }

            ~DescriptorAllocator() = default;

        private:
            memory::LinearAllocator mSections;
            memory::LinearAllocator mStaticDescriptors;
            memory::RingAllocator mDynamicDescriptorsGPU;

            memory::RingAllocator mDynamicDescriptorsCPU;

            memory::RingAllocator mRenderTargetDescriptors;
            memory::RingAllocator mDepthStencilDescriptors;

            std::array<std::mutex, DescriptorsCategory::COUNT> mMutex;
        };

        DescriptorAllocator mAllocator;

    private:
        DescriptorHeap mShaderViewsDescriptorHeapCPU;
        DescriptorHeap mShaderViewsDescriptorHeapGPU;
        DescriptorHeap mRenderTargetViewsDescriptorHeap;
        DescriptorHeap mDepthStencilViewsDescriptorHeap;
    };
}
