#pragma once

#include "common.h"
#include "Log.h"

#include "dx12/Device.h"

#include "dx12/DescriptorHeap.h"

#include "memory/RingAllocator.h"
#include "memory/LinearAllocator.h"

namespace graphics
{
    class DescriptorHeapWithAllocator
    {
    public:
        enum Category
        {
            StaticGPU,
            DynamicCPU,
            DynamicGPU,
            RenderTargetCPU,

            COUNT
        };

    public:
        DescriptorHeapWithAllocator(dx12::Device& device);

        dx12::DescriptorHandle allocate(const Category category = Category::DynamicCPU, const size_t count = 1);

        // Batched copy of descriptors into this heap from one or more source ranges.
        void copyDescriptorsFrom(dx12::Device& device, const dx12::DescriptorHandle& destStart,
                                 const std::vector<dx12::DescriptorHandle>& srcStarts,
                                 const std::vector<unsigned int>& srcCounts,
                                 const Category category = Category::DynamicCPU);

        inline dx12::DescriptorHeap& getDescriptorHeap(const Category category)
        {
            switch (category)
            {
            default:
                HASSERT_SOFT(false, "Invalid category(%d) for descriptor heap. Using ShaderViewsDescriptorHeap.", category);
            case Category::DynamicCPU:
                return mShaderViewsDescriptorHeapCPU;
            case Category::StaticGPU:
            case Category::DynamicGPU:
                return mShaderViewsDescriptorHeapGPU;
            case Category::RenderTargetCPU:
                return mRenderTargetViewsDescriptorHeap;
            }
        }

    private:
        dx12::Device& mDevice;

    private:
        class DescriptorAllocator
        {
        public:
            static constexpr size_t MEMORY_BUDGET[Category::COUNT] = {
                memory::MB(1), // Category::Static
                memory::MB(1), // Category::DynamicCPU
                memory::MB(1), // Category::DynamicGPU
                memory::MB(1) // Category::RenderTargets
            };

            DescriptorAllocator()
                : mSections(memory::MB(2)),
                mStaticDescriptors(MEMORY_BUDGET[Category::StaticGPU], mSections.allocate(MEMORY_BUDGET[Category::StaticGPU])),
                mDynamicDescriptorsCPU(MEMORY_BUDGET[Category::DynamicCPU]),
                mDynamicDescriptorsGPU(MEMORY_BUDGET[Category::DynamicGPU], mSections.allocate(MEMORY_BUDGET[Category::DynamicGPU])),
                mRenderTargetDescriptors(MEMORY_BUDGET[Category::RenderTargetCPU])
            {}

            inline const size_t allocate(const Category section, const size_t sizeInBytes)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[section]);

                switch (section)
                {
                default:
                    HASSERT_SOFT(false, "Invalid section(%d) for descriptor allocation. Using DynamicCPU section.", section);
                case Category::DynamicCPU:
                    return mDynamicDescriptorsCPU.allocate(sizeInBytes);
                case Category::StaticGPU:
                    return mStaticDescriptors.allocate(sizeInBytes);
                case Category::DynamicGPU:
                    return mDynamicDescriptorsGPU.allocate(sizeInBytes);
                case Category::RenderTargetCPU:
                    return mRenderTargetDescriptors.allocate(sizeInBytes);
                }
            }

            inline void reset(const Category section)
            {
                std::lock_guard<std::mutex> scopedLockGuard(mMutex[section]);

                switch (section)
                {
                default:
                    HASSERT(false, "Invalid section(%d) for descriptor allocation.", section);
                case Category::DynamicCPU:
                    mDynamicDescriptorsCPU.reset();
                    break;
                case Category::StaticGPU:
                    mStaticDescriptors.reset();
                    break;
                case Category::DynamicGPU:
                    mDynamicDescriptorsGPU.reset();
                    break;
                case Category::RenderTargetCPU:
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

            std::mutex mMutex[Category::COUNT];
        };

        DescriptorAllocator mAllocator;

    private:
        dx12::DescriptorHeap mShaderViewsDescriptorHeapCPU;
        dx12::DescriptorHeap mShaderViewsDescriptorHeapGPU;
        dx12::DescriptorHeap mRenderTargetViewsDescriptorHeap;
    };
}
