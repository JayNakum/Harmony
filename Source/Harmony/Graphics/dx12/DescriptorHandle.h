#pragma once

#include "common.h"
#include "directx.h"

namespace graphics
{
	namespace dx12
	{
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
	}
}
