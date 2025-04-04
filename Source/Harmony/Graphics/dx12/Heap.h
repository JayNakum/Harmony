#pragma once

#include "common.h"
#include "directx.h"

#include "HeapEnums.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * Wrapper over ID3D12Heap
        */
        class Heap
        {
        public:
            struct Description
            {
                std::string name;
                size_t sizeInBytes;
                HeapType type;
            };

            // Creates and initializes an ID3D12Heap
            Heap(const Description& createDescription);

            // Creates space for size inside the heap.
            // Returns the heapOffset that points to the start of the allocation.
            // The offset can then be used to access the resources.
            uint64_t allocate(const uint64_t sizeInBytes);

            // Resets the offset and available size.
            // Next allocations will overwrite the previous ones.
            void reset();

            // Frees the heap.
            // Since mHeap is a ComPtr the heap is released automatically.
            ~Heap();

        public:
            inline const HeapType getHeapType() const { return mDescription.type; }
            inline const Description& getDescription() const { return mDescription; }

            // Checks for available space inside heap.
            inline bool canAllocate(const uint64_t sizeInBytes) const { return sizeInBytes <= mFreeSpace; }

            // Returns the remaining free size in bytes
            inline uint64_t getFreeSpace() const { return mFreeSpace; }

            // Returns the raw pointer of ID3D12Heap.
            inline const ID3D12Heap* get() const { return mHeap.Get(); }
            inline ID3D12Heap* get() { return mHeap.Get(); }

            // Operator overload that returns the raw pointer of ID3D12Heap.
            inline const ID3D12Heap* operator->() const { return mHeap.Get(); }
            inline ID3D12Heap* operator->() { return mHeap.Get(); }

            static D3D12_HEAP_PROPERTIES toHeapProperties(const Description& description);

        private:
            const Description mDescription;
            Microsoft::WRL::ComPtr<ID3D12Heap> mHeap;

            // Keep track of the free spaces available for validation.
            uint64_t mFreeSpace;

            // Handle where the next allocation will be done.
            uint64_t mNextAllocationOffset;
        };
    }
}
