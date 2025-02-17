#pragma once

#include "common.h"
#include "directx.h"

#include "DescriptorHeapEnums.h"
#include "DescriptorHandle.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * A wrapper over ID3D12DescriptorHeap.
        * Descriptor Heaps can be either manually created with fixed size using this class;
        * or can be created using DescriptorHeapPool class.
        *
        * Recommended to use this class directly for smaller heaps (for example heaps with type RTV or DSV).
        * Recommended to use this class for CPU only heaps.
        * For other use cases, use the DescriptorHeapPool directly.
        */
        class DescriptorHeap
        {
        public:
            struct Description
            {
                std::string name;
                DescriptorHeapType type;
                size_t maxDescriptors;
                DescriptorHeapFlag flag = DescriptorHeapFlag::ShaderVisible;
            };

            // Create and initialize the descriptor heap.
            DescriptorHeap(const Description& createDescription);

            // Creates space for count number of descriptors inside the heap.
            // Returns the handle that points to the start of the allocation.
            // The handle will then be used for actual allocation.
            DescriptorHandle allocate(const size_t count = 1);

            // Resets the heap by setting the allocation handle at start and numFreeDescriptors to maxDescriptors
            void reset();

            // Frees the heap.
            // Since mHeap is a ComPtr the heap is released automatically.
            ~DescriptorHeap();

        public:
            inline const DescriptorHeapType getDescriptorHeapType() const { return mDescription.type; }
            inline const DescriptorHeapFlag getDescriptorHeapFlag() const { return mDescription.flag; }

            // Checks for available space inside heap.
            inline bool canAllocate(const size_t count) const { return count <= mNumFreeDescriptors; }

            // Checks for three conditions.
            // - Whether the handle falls inside the range between firstHandle and heapSize
            // - Whether the handle type matches with the HeapFlag
            // - If descriptor is ShaderVisible then whether the GPU offset and CPU offset matches.
            bool validateHandle(const DescriptorHandle& dHandle) const;

            // Operator overload for array like indexing.
            inline DescriptorHandle operator[] (size_t index) const { return mFirstHandle + index * mDescriptorSize; }

            // Calculates the heap offset of the handle.
            inline uint32_t getOffsetOfHandle(const DescriptorHandle& dHandle) const
            {
                return (uint32_t)((dHandle.getCPUPtr() - mFirstHandle.getCPUPtr()) / mDescriptorSize);
            }

            // Return descriptor handle increment size.
            inline const size_t getDescriptorSize() const { return mDescriptorSize; }

            // Returns the raw pointer of ID3D12DescriptorHeap.
            inline const ID3D12DescriptorHeap* get() const { return mHeap.Get(); }
            inline ID3D12DescriptorHeap* get() { return mHeap.Get(); }

            // Operator overload that returns the raw pointer of ID3D12DescriptorHeap.
            inline const ID3D12DescriptorHeap* operator->() const { return mHeap.Get(); }
            inline ID3D12DescriptorHeap* operator->() { return mHeap.Get(); }

        public:
            // maps graphics::DescriptorHeapType and D3D12_DESCRIPTOR_HEAP_TYPE
            static const std::unordered_map<graphics::DescriptorHeapType, D3D12_DESCRIPTOR_HEAP_TYPE> descriptorHeapType;

            // maps graphics::DescriptorHeapFlag and D3D12_DESCRIPTOR_HEAP_FLAGS
            static const std::unordered_map<graphics::DescriptorHeapFlag, D3D12_DESCRIPTOR_HEAP_FLAGS> descriptorHeapFlag;

        private:
            const Description mDescription;

            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mHeap;
            D3D12_DESCRIPTOR_HEAP_DESC mHeapDesc;

            // Queried from device via GetDescriptorHandleIncrementSize
            size_t mDescriptorSize;

            // Keep track of the free spaces available for validation.
            size_t mNumFreeDescriptors;

            // Heap start handle.
            DescriptorHandle mFirstHandle;

            // Handle where the next allocation will be done.
            DescriptorHandle mNextFreeHandle;
        };
    }
}
