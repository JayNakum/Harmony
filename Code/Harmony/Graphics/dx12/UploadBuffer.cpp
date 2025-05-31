#include "UploadBuffer.h"

#include "dx12/Heap.h"
#include "dx12/types/GPUResourceStates.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        UploadBuffer::UploadBuffer(Device& device, const Description& createDescription)
            : GPUResource(GPUResourceStates::GenericRead),
            mDescription(createDescription)
        {
            destroy();

            // Create an upload buffer. This is CPU-visible, but it's write combined memory,
            // so avoid reading back from it.
            static Heap::Description description{};
            description.name = "temp upload heap description";
            description.sizeInBytes = 0;
            description.type = HeapType::Upload;
            static D3D12_HEAP_PROPERTIES heapProperties = Heap::toD3DHeapProperties(description);

            // Upload buffers must be 1-dimensional
            D3D12_RESOURCE_DESC createDesc = GPUResource::toD3DResourceDesc(describeResource());

            HRESULT hResult = device->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &createDesc,
                dx12::toD3DResourceStates(mUsageState), nullptr, IID_PPV_ARGS(&mResource));

            HASSERT(SUCCEEDED(hResult), "Failed to create upload buffer %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            setD3DName(mResource.Get(), utility::toWideString(createDescription.name).c_str());
        }

        const GPUResource::Description UploadBuffer::describeResource() const
        {
            GPUResource::Description description{};

            description.name = mDescription.name;
            description.width = mDescription.bufferSize;
            description.dimension = GPUResourceDimension::Buffer;
            return description;
        }

        void* UploadBuffer::map()
        {
            void* memory = nullptr;
            CD3DX12_RANGE range = CD3DX12_RANGE(0, mDescription.bufferSize);
            HRESULT hResult = mResource->Map(0, &range, &memory);
            HASSERT(SUCCEEDED(hResult), "Failed to map upload buffer resource.\n%s", utility::translateErrorCode(hResult));
            return memory;
        }

        void UploadBuffer::unmap(const size_t begin, const size_t end)
        {
            CD3DX12_RANGE range = CD3DX12_RANGE(begin, std::min(end, mDescription.bufferSize));
            mResource->Unmap(0, &range);
        }


        void UploadBuffer::createConstantBufferView(Device& device, DescriptorHandle& cbvHandle) const
        {
            size_t size = maths::alignUp(mDescription.bufferSize, 16);

            createConstantBufferView(device, cbvHandle, 0, size);
        }

        void UploadBuffer::createConstantBufferView(Device& device, DescriptorHandle& cbvHandle, size_t size, const uint32_t offset) const
        {
            HASSERT(offset + size <= mDescription.bufferSize, "offset(%d) + size(%d) is more than total buffer size(%d).", offset, size, mDescription.bufferSize);

            size = maths::alignUp(size, 16);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
            cbvDesc.BufferLocation = getGPUVirtualAddress() + (size_t)offset;
            cbvDesc.SizeInBytes = (UINT)size;

            device->CreateConstantBufferView(&cbvDesc, cbvHandle);
        }
        void UploadBuffer::createShaderResourceView(Device& device, DescriptorHandle& srvHandle) const
        {
            createShaderResourceView(device, srvHandle, mDescription.bufferSize);
        }

        void UploadBuffer::createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const uint64_t sizeInBytes, const unsigned int offsetInBytes) const
        {
            HASSERT(offsetInBytes + sizeInBytes <= mDescription.bufferSize, "offset(%d) + size(%d) is more than total buffer size(%d).", offsetInBytes, sizeInBytes, mDescription.bufferSize);

            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

            // GPU views into a UploadBuffer are in units of 4 bytes.
            srvDesc.Buffer.FirstElement = (UINT)offsetInBytes / 4;
            srvDesc.Buffer.NumElements = (UINT)sizeInBytes / 4;

            srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

            device->CreateShaderResourceView(mResource.Get(), &srvDesc, srvHandle);
        }

        void UploadBuffer::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle) const
        {
            createUnorderedAccessView(device, uavHandle, mDescription.bufferSize);
        }

        void UploadBuffer::createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle, const uint64_t sizeInBytes, const unsigned int offsetInBytes) const
        {
            HASSERT(offsetInBytes + sizeInBytes <= mDescription.bufferSize, "offset(%d) + size(%d) is more than total buffer size(%d).", offsetInBytes, sizeInBytes, mDescription.bufferSize);

            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;

            // GPU views into a UploadBuffer are in units of 4 bytes.
            uavDesc.Buffer.FirstElement = (UINT)offsetInBytes / 4;
            uavDesc.Buffer.NumElements = (UINT)sizeInBytes / 4;

            uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

            device->CreateUnorderedAccessView(mResource.Get(), nullptr, &uavDesc, uavHandle);
        }
    }
}
