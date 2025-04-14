#pragma once

#include "common.h"
#include "directx.h"

#include "HeapEnums.h"

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

            // Frees the heap.
            // Since mHeap is a ComPtr the heap is released automatically.
            ~Heap();

        public:
            // Returns the raw pointer of ID3D12Heap.
            inline const ID3D12Heap* get() const { return mHeap.Get(); }
            inline ID3D12Heap* get() { return mHeap.Get(); }

            // Operator overload that returns the raw pointer of ID3D12Heap.
            inline const ID3D12Heap* operator->() const { return mHeap.Get(); }
            inline ID3D12Heap* operator->() { return mHeap.Get(); }

        public:
            static D3D12_HEAP_PROPERTIES toD3DHeapProperties(const Description& description);

        private:
            Microsoft::WRL::ComPtr<ID3D12Heap> mHeap;
        };
    }
}
