#pragma once

#include "common.h"

#include "directx.h"

#include "GFXCommandContext.h"

namespace graphics
{
    /*
    * Type of resources in the heap.
    */
    enum DescriptorHeapType
    {
        View, // CBV_SRV_UAV
        Sampler,
        RenderTarget,
        DepthStencil,

        DescriptorHeapTypeCount
    };

    /*
    * Descriptor heap visibility.
    */
    enum DescriptorHeapFlag
    {
        None, // CPU Only
        ShaderVisible, // CPU and GPU both

        DescriptorHeapFlagCount
    };

    /*
    * This handle refers to a descriptor or a descriptor table(contiguous descriptors)
    * A simple abstraction over CPU and GPU handles with helper functions and operator overloads to use them easily.
    * The handle can be None(CPU Only) and Shader Visible(CPU and GPU both).
    * And just like DescriptorHeapFlag, handles can never be GPU only.
    */
    class DescriptorHandle
    {
    public:
        static constexpr uint32_t INVALID_ADDRESS = -1;

        DescriptorHandle()
        {
            mCPUHandle.ptr = INVALID_ADDRESS;
            mGPUHandle.ptr = INVALID_ADDRESS;
        }

        /*
        * Allows constructing handles that might not be shader visible.
        */
        DescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
            : mCPUHandle(cpuHandle)
        {
            mGPUHandle.ptr = INVALID_ADDRESS;
        }

        DescriptorHandle(const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
            : mCPUHandle(cpuHandle), mGPUHandle(gpuHandle)
        {}

        DescriptorHandle operator+ (const INT offsetScaledByDescriptorSize) const
        {
            DescriptorHandle ret = *this;
            ret += offsetScaledByDescriptorSize;
            return ret;
        }

        void operator += (const INT offsetScaledByDescriptorSize)
        {
            if (mCPUHandle.ptr != INVALID_ADDRESS)
            {
                mCPUHandle.ptr += offsetScaledByDescriptorSize;
            }

            if (mGPUHandle.ptr != INVALID_ADDRESS)
            {
                mGPUHandle.ptr += offsetScaledByDescriptorSize;
            }
        }

        const D3D12_CPU_DESCRIPTOR_HANDLE* operator&() const { return &mCPUHandle; }
        operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return mCPUHandle; }
        operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return mGPUHandle; }

        inline size_t getCPUPtr() const { return mCPUHandle.ptr; }
        inline uint64_t getGPUPtr() const { return mGPUHandle.ptr; }
        inline bool isNull() const { return mCPUHandle.ptr == INVALID_ADDRESS; }
        inline bool isShaderVisible() const { return mGPUHandle.ptr != INVALID_ADDRESS; }

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE mCPUHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE mGPUHandle;
    };

    /*
    * A wrapper over ID3D12DescriptorHeap.
    * Descriptor Heaps can be either manually created with fixed size using this class;
    * or can be created using DescriptorHeapPool class.
    *
    * Recommended to use this class directly for smaller heaps for example heaps with type RTV or DSV.
    * Recommended to use this class for CPU only heaps.
    */
    class DescriptorHeap
    {
        friend class DescriptorHeapPool;
    public:
        struct Description
        {
            std::string name;
            DescriptorHeapType type;
            size_t maxDescriptors;
            DescriptorHeapFlag flag = DescriptorHeapFlag::ShaderVisible;
        };

        /*
        * Create and initialize the descriptor heap.
        */
        DescriptorHeap(const Description& createDescription);

        /*
        * Frees the heap. Since mHeap is a ComPtr the heap is released automatically.
        */
        ~DescriptorHeap();

        /*
        * Checks for available space inside heap.
        */
        inline bool canAllocate(const size_t count) const { return count <= mNumFreeDescriptors; }

        /*
        * Creates space for count number of descriptors inside the heap.
        * Returns the handle that points to the start of the allocation.
        * The handle will then be used for actual allocation.
        */
        DescriptorHandle allocate(const size_t count = 1);

        /*
        * Operator overload for array like indexing.
        */
        DescriptorHandle operator[] (size_t index) const { return mFirstHandle + index * mDescriptorSize; }

        /*
        * Calculates the heap offset of the handle.
        */
        inline uint32_t getOffsetOfHandle(const DescriptorHandle& dHandle) const
        {
            return (uint32_t)((dHandle.getCPUPtr() - mFirstHandle.getCPUPtr()) / mDescriptorSize);
        }

        /*
        * Resets the heap by setting the allocation handle at start and numFreeDescriptors to maxDescriptors
        */
        void reset();

        /*
        * Checks for three conditions.
        * - Whether the handle falls inside the range between firstHandle and heapSize
        * - Whether the handle type matches with the HeapFlag
        * - If descriptor is ShaderVisible then whether the GPU offset and CPU offset matches.
        */
        bool validateHandle(const DescriptorHandle& dHandle) const;

        /*
        * Return descriptor handle increment size.
        */
        inline const size_t getDescriptorSize() const { return mDescriptorSize; }

        /*
        * Operator overload that returns the raw pointer of ID3D12DescriptorHeap.
        */
        ID3D12DescriptorHeap* operator->() const
        {
            return mHeap.Get();
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mHeap;
        D3D12_DESCRIPTOR_HEAP_DESC mHeapDesc;

        /*
        * Queried from device via GetDescriptorHandleIncrementSize
        */
        size_t mDescriptorSize;

        /*
        * Keep track of the free spaces available for validation.
        */
        size_t mNumFreeDescriptors;

        /*
        * Heap start handle.
        */
        DescriptorHandle mFirstHandle;

        /*
        * Handle where the next allocation will be done.
        */
        DescriptorHandle mNextFreeHandle;
    };
}
