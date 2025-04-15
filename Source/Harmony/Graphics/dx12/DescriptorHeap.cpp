#include "DescriptorHeap.h"

#include "types/DescriptorHeapType.h"
#include "types/DescriptorHeapFlags.h"

namespace graphics
{
    namespace dx12
    {
        DescriptorHeap::DescriptorHeap(Device& device, const Description& createDescription)
            : mMaxDescriptors(createDescription.maxDescriptors), mType(createDescription.type), mDescriptorSize(0), mHeap(nullptr)
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
            heapDesc.Type = toD3DDescriptorHeapType(createDescription.type);
            heapDesc.NumDescriptors = (UINT)createDescription.maxDescriptors;
            heapDesc.Flags = toD3DDescriptorHeapFlags(createDescription.flag);
            heapDesc.NodeMask = 1;

            HRESULT hResult = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mHeap.ReleaseAndGetAddressOf()));
            HASSERT(SUCCEEDED(hResult), "Failed to create descriptor heap.\n%s", utility::translateErrorCode(hResult));

            setName(mHeap.Get(), utility::toWideString(createDescription.name).c_str());

            mDescriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);

            switch (createDescription.flag)
            {
            default:
            case DescriptorHeapFlags::GPUVisible:
            {
                mHeapStart = DescriptorHandle(mHeap->GetCPUDescriptorHandleForHeapStart(),
                    mHeap->GetGPUDescriptorHandleForHeapStart());
                break;
            }
            case DescriptorHeapFlags::CPUVisible:
            {
                mHeapStart = DescriptorHandle(mHeap->GetCPUDescriptorHandleForHeapStart());
                break;
            }
            }
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
}
