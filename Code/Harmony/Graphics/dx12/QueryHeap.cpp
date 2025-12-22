#include "QueryHeap.h"

#include "types/HeapType.h"

namespace graphics
{
    QueryHeap::QueryHeap(Device& device, const Description& createDescription)
    {
        D3D12_QUERY_HEAP_DESC heapDesc{};
        heapDesc.Count = static_cast<UINT>(createDescription.count);
        if (createDescription.type == QueryHeapType::CopyTimestamp)
        {
            HASSERT(device.isCopyQueueTimestampQueriesSupported(), "CopyTimestamp not supported.");
        }
        heapDesc.Type = dx12::toD3DQueryHeapType(createDescription.type);

        HRESULT hResult = device->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&mQueryHeap));
        HASSERT(SUCCEEDED(hResult), "Failed to create heap for opaque render items.\n", utility::translateErrorCode(hResult).c_str());

        setD3DName(mQueryHeap.Get(), utility::toWideString(createDescription.name).c_str());
    }

    QueryHeap::~QueryHeap()
    {
        mQueryHeap = nullptr;
    }
}
