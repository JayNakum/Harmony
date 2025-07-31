#include "DescriptorHeap.h"

#include "types/DescriptorHeapType.h"
#include "types/DescriptorHeapFlags.h"

namespace graphics
{
    DescriptorHeap::DescriptorHeap(Device& device, const Description& createDescription)
        : mMaxDescriptors(createDescription.maxDescriptors), mType(createDescription.type), mDescriptorSize(0), mHeap(nullptr)
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.Type = dx12::toD3DDescriptorHeapType(createDescription.type);
        heapDesc.NumDescriptors = (UINT)createDescription.maxDescriptors;
        heapDesc.Flags = dx12::toD3DDescriptorHeapFlags(createDescription.flag);
        heapDesc.NodeMask = 1;

        HRESULT hResult = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mHeap.ReleaseAndGetAddressOf()));
        HASSERT(SUCCEEDED(hResult), "Failed to create descriptor heap.\n%s", utility::translateErrorCode(hResult));

        setD3DName(mHeap.Get(), utility::toWideString(createDescription.name).c_str());

        mDescriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);

        switch (createDescription.flag)
        {
        default:
        case DescriptorHeapFlags::CPUVisible:
        {
            mHeapStart = DescriptorHandle(mHeap->GetCPUDescriptorHandleForHeapStart());
            break;
        }
        case DescriptorHeapFlags::GPUVisible:
        {
            mHeapStart = DescriptorHandle(mHeap->GetCPUDescriptorHandleForHeapStart(),
                mHeap->GetGPUDescriptorHandleForHeapStart());
            break;
        }
        }
    }

    void DescriptorHeap::copyDescriptorsFrom(
        Device& device,
        const DescriptorHandle& destStart,
        const std::vector<DescriptorHandle>& srcStarts,
        const std::vector<unsigned int>& srcCounts)
    {
        HASSERT(srcStarts.size() == srcCounts.size(), "Mismatched srcStarts and srcCounts size");

        const UINT numSrcRanges = static_cast<UINT>(srcStarts.size());

        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcHandles(numSrcRanges);
        for (UINT i = 0; i < numSrcRanges; ++i)
        {
            srcHandles[i] = srcStarts[i];
        }

        device->CopyDescriptors(
            1, // Number of destination ranges
            &destStart, // Destination base handle
            &srcCounts[0], // Number of descriptors in destination range
            numSrcRanges, // Number of source ranges
            srcHandles.data(), // Source base handles
            srcCounts.data(), // Number of descriptors per source range
            dx12::toD3DDescriptorHeapType(mType) // Descriptor heap type
        );
    }

    bool DescriptorHeap::validateHandle(const DescriptorHandle& dHandle) const
    {
        if (dHandle.getCPUPtr() < mHeapStart.getCPUPtr() ||
            dHandle.getCPUPtr() >= mHeapStart.getCPUPtr() + mMaxDescriptors * mDescriptorSize)
        {
            return false;
        }

        if (dHandle.isGPUVisible() != mHeapStart.isGPUVisible())
        {
            return false;
        }

        if (dHandle.isGPUVisible())
        {
            if (dHandle.getGPUPtr() - mHeapStart.getGPUPtr() !=
                dHandle.getCPUPtr() - mHeapStart.getCPUPtr())
            {
                return false;
            }
        }

        return true;
    }

    DescriptorHeap::~DescriptorHeap()
    {
        mHeap = nullptr;
    }
}
