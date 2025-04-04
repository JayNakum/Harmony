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
        */
        class DescriptorHeap
        {
        public:
            struct Description
            {
                std::string name;
                DescriptorHeapType type;
                size_t maxDescriptors;
                DescriptorHeapFlags flag;
            };

            // Create and initialize the descriptor heap.
            DescriptorHeap(const Description& createDescription);

            // Get the heap start handle
            inline DescriptorHandle getHeapStart() const { return mHeapStart; }

            // Operator overload for array like indexing.
            inline DescriptorHandle operator[] (size_t index) const { return mHeapStart + (INT)index * (INT)mDescriptorSize; }

            // Calculates the heap offset of the handle.
            inline uint32_t getOffsetOfHandle(const DescriptorHandle& dHandle) const
            {
                return (uint32_t)((dHandle.getCPUPtr() - mHeapStart.getCPUPtr()) / mDescriptorSize);
            }

            // Checks for three conditions.
            // - Whether the handle falls inside the range between firstHandle and heapSize
            // - Whether the handle type matches with the HeapFlag
            // - If descriptor is GPUVisible then whether the GPU offset and CPU offset matches.
            bool validateHandle(const DescriptorHandle& dHandle) const;

            // Frees the heap.
            // Since mHeap is a ComPtr the heap is released automatically.
            virtual ~DescriptorHeap();

        public:
            inline const DescriptorHeapType getDescriptorHeapType() const { return mDescription.type; }
            inline const DescriptorHeapFlags getDescriptorHeapFlags() const { return mDescription.flag; }

            // Return descriptor handle increment size.
            inline const size_t getDescriptorSize() const { return mDescriptorSize; }

            // Returns the raw pointer of ID3D12DescriptorHeap.
            inline const ID3D12DescriptorHeap* get() const { return mHeap.Get(); }
            inline ID3D12DescriptorHeap* get() { return mHeap.Get(); }

            // Operator overload that returns the raw pointer of ID3D12DescriptorHeap.
            inline const ID3D12DescriptorHeap* operator->() const { return mHeap.Get(); }
            inline ID3D12DescriptorHeap* operator->() { return mHeap.Get(); }

        protected:
            const Description mDescription;

            // Queried from device via GetDescriptorHandleIncrementSize
            size_t mDescriptorSize;

            // Heap start handle.
            dx12::DescriptorHandle mHeapStart;

        private:
            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mHeap;
        };
    }
}
