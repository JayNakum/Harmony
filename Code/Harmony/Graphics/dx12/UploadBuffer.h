#pragma once

#include "common.h"
#include "directx.h"

#include "Device.h"

#include "GPUResource.h"
#include "DescriptorHandle.h"

namespace graphics
{
    namespace dx12
    {
        /*
        * A Committed resource in UPLOAD_HEAP.
        * The reason why it is a committed resource because usually we only using UploadBuffer as an
        * intermediate before we copy the data to a DEFAULT_HEAP.
        * In future if, for some reason, we nead a placed resource in UPLOAD_HEAP then will // TODO: implement it.
        */
        class UploadBuffer : public dx12::GPUResource
        {
        public:
            struct Description
            {
                std::string name;
                size_t bufferSize;
            };

            // Creates a committed buffer of size Description::bufferSize
            UploadBuffer(Device& device, const Description& createDescription);

            // Creates a filtered and valid GPUResource::Description based on the initData
            const GPUResource::Description describeResource() const override;

            // Maps the ID3D12Resource and returns the memory pointer.
            void* map();

            // Unmaps the resource from begin to end/mBufferSize
            void unmap(const size_t begin = 0, const size_t end = -1);

            inline const size_t getSize() const { return mDescription.bufferSize; }

            // Creates a CBV based on buffer description.
            // Requires a pre-allocated descriptorHandle& from a descriptor heap.
            void createConstantBufferView(Device& device, DescriptorHandle& cbvHandle) const;
            void createConstantBufferView(Device& device, DescriptorHandle& cbvHandle, size_t size, const uint32_t offset = 0) const;

            void createShaderResourceView(Device& device, DescriptorHandle& srvHandle) const;
            // Creates an SRV with custom size and offset.
            // Requires a pre-allocated descriptorHandle& from a descriptor heap.
            void createShaderResourceView(Device& device, DescriptorHandle& srvHandle, const uint64_t sizeInBytes, const unsigned int offsetInBytes = 0) const;

            void createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle) const;
            // Creates an UAV with custom size and offset.
            // Requires a pre-allocated descriptorHandle& from a descriptor heap.
            void createUnorderedAccessView(Device& device, DescriptorHandle& uavHandle, const uint64_t sizeInBytes, const unsigned int offsetInBytes = 0) const;

            // Calls destroy which frees the ComPtr<ID3D12Resource>
            virtual ~UploadBuffer()
            {
                destroy();
            }

        private:
            const Description mDescription;
        };
    }
}
