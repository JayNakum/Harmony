#include "GPUBuffer.h"

#include "Renderer.h"

namespace graphics
{
    namespace dx12
    {
        void GPUBuffer::createCommitted(const Description& createDescription, const void* initialData)
        {
            static const CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

            initialize(createDescription);
            D3D12_RESOURCE_DESC resourceDescription = describeBuffer();

            HRESULT hResult = GfxDevice()->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDescription, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(getAddressOf())
            );
        }
    }
}
