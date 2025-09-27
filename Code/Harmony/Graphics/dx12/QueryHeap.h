#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

#include "Device.h"

#include "HeapEnums.h"

namespace graphics
{
    /*
    * Wrapper over ID3D12QueryHeap
    */
    class QueryHeap
    {
    public:
        struct Description
        {
            std::string name;
            size_t count;
            QueryHeapType type;
        };

        // Creates and initializes an ID3D12QueryHeap
        QueryHeap(Device& device, const Description& createDescription);

        // Frees the query heap.
        // Since mQueryHeap is a ComPtr the heap is released automatically.
        ~QueryHeap();

    public:
        // Returns the raw pointer of ID3D12QueryHeap.
        inline const ID3D12QueryHeap* get() const { return mQueryHeap.Get(); }
        inline ID3D12QueryHeap* get() { return mQueryHeap.Get(); }

        // Operator overload that returns the raw pointer of ID3D12QueryHeap.
        inline const ID3D12QueryHeap* operator->() const { return mQueryHeap.Get(); }
        inline ID3D12QueryHeap* operator->() { return mQueryHeap.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12QueryHeap> mQueryHeap;
    };
}
