#pragma once

#include "common.h"
#include "Log.h"
#include "directx.h"

namespace graphics
{
    /*
    * This handle refers to a descriptor or a descriptor table(contiguous descriptors)
    * A simple abstraction over CPU and GPU handles with helper functions and operator overloads to use them easily.
    * The handle can be None(CPU Only) and Shader Visible(CPU and GPU both).
    * And just like DescriptorHeapFlags, handles can never be GPU only.
    */
    class DescriptorHandle
    {
    public:
        static constexpr uint32_t INVALID_ADDRESS = 0x1deadb0b;

        DescriptorHandle()
        {
            mCPUHandle.ptr = INVALID_ADDRESS;
            mGPUHandle.ptr = INVALID_ADDRESS;
        }

        // Allows constructing handles that might not be shader visible.
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

        inline bool operator ==(const DescriptorHandle& other) const
        {
            return (mCPUHandle.ptr == other.getCPUPtr());
        }
        inline bool operator !=(const DescriptorHandle& other) const
        {
            return (mCPUHandle.ptr != other.getCPUPtr());
        }

        inline size_t getCPUPtr() const { return mCPUHandle.ptr; }
        inline uint64_t getGPUPtr() const { return mGPUHandle.ptr; }

        inline bool isNull() const { return mCPUHandle.ptr == INVALID_ADDRESS; }
        inline bool isGPUVisible() const { return mGPUHandle.ptr != INVALID_ADDRESS; }

        const D3D12_CPU_DESCRIPTOR_HANDLE getCPUHandle() const { return mCPUHandle; }
        const D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle() const { return mGPUHandle; }

        const D3D12_CPU_DESCRIPTOR_HANDLE* operator&() const { return &mCPUHandle; }
        operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return mCPUHandle; }

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE mCPUHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE mGPUHandle;
    };
}
