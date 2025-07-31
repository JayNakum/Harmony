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
            // 4 MB TOTAL.
            // 2 MB for GPU heap (StaticGPU + DynamicGPU).
            // 1 MB for CPU heap.
            // 1 MB for RTV CPU heap.
            static constexpr size_t MEMORY_BUDGET[DescriptorsCategory::COUNT] = {
                memory::MB(1), // DescriptorsCategory::StaticGPU
                memory::MB(1), // DescriptorsCategory::DynamicCPU
                memory::MB(1), // DescriptorsCategory::DynamicGPU
                memory::MB(1) // DescriptorsCategory::RenderTargets
            };

            DescriptorAllocator()
                : mSections(memory::MB(2)),
                mStaticDescriptors(MEMORY_BUDGET[DescriptorsCategory::StaticGPU], mSections.allocate(MEMORY_BUDGET[DescriptorsCategory::StaticGPU])),
                mDynamicDescriptorsCPU(MEMORY_BUDGET[DescriptorsCategory::DynamicCPU]),
                mDynamicDescriptorsGPU(MEMORY_BUDGET[DescriptorsCategory::DynamicGPU], mSections.allocate(MEMORY_BUDGET[DescriptorsCategory::DynamicGPU])),
                mRenderTargetDescriptors(MEMORY_BUDGET[DescriptorsCategory::RenderTargetCPU])
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
                }
            }

            ~DescriptorAllocator() = default;

        private:
            memory::LinearAllocator mSections;
            memory::LinearAllocator mStaticDescriptors;
            memory::RingAllocator mDynamicDescriptorsGPU;

            memory::RingAllocator mDynamicDescriptorsCPU;

            memory::RingAllocator mRenderTargetDescriptors;

            std::mutex mMutex[DescriptorsCategory::COUNT];
        };

        DescriptorAllocator mAllocator;

    private:
        DescriptorHeap mShaderViewsDescriptorHeapCPU;
        DescriptorHeap mShaderViewsDescriptorHeapGPU;
        DescriptorHeap mRenderTargetViewsDescriptorHeap;
    };
}
