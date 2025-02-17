#include "GPUBuffer.h"

namespace graphics
{
    namespace dx12
    {
        void GPUBuffer::createCommitted(const std::string& name, uint32_t numElements, uint32_t elementSize, const void* initialData)
        {
            destroy();
            
        }
        
        void GPUBuffer::createCommitted(const std::string & name, uint32_t numElements, uint32_t elementSize, const UploadBuffer & sourceData, uint32_t sourceOffset)
        {}
        
        void GPUBuffer::createPlaced(const std::string & name, Heap & backingHeap, uint64_t heapOffset, uint32_t numElements, uint32_t elementSize, const void* initialData)
        {}

        void GPUBuffer::createPlaced(const std::string & name, Heap & backingHeap, uint64_t heapOffset, uint32_t numElements, uint32_t elementSize, const UploadBuffer & sourceData, uint32_t sourceOffset)
        {}
    }
}
