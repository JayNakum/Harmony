#pragma once

#include "common.h"

#include "dx12/GPUResource.h"

namespace graphics
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
        UploadBuffer(const Description& createDescription);

        // Maps the ID3D12Resource and returns the memory pointer.
        void* map();

        // Unmaps the resource from begin to end/mBufferSize
        void unmap(size_t begin = 0, size_t end = -1);

        inline const size_t getSize() const { return mBufferSize; }

        // Calls destroy which frees the ComPtr<ID3D12Resource>
        virtual ~UploadBuffer()
        {
            destroy();
        }

    private:
        const size_t mBufferSize;
    };
}
