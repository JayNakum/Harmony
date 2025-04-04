#include "DescriptorHeap.h"

#include "Renderer.h"

#include "types/DescriptorHeapFlags.h"
#include "types/DescriptorHeapType.h"

namespace graphics
{
    namespace dx12
    {
        DescriptorHeap::DescriptorHeap(const Description & createDescription)
            : mDescription(createDescription), mHeap(nullptr), mDescriptorSize(0), mNumFreeDescriptors(0)
        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
            heapDesc.Type = descriptorHeapType.at(mDescription.type);
            heapDesc.NumDescriptors = (UINT)mDescription.maxDescriptors;
            heapDesc.Flags = descriptorHeapFlag.at(mDescription.flag);
            heapDesc.NodeMask = 1;

            HRESULT hResult = GfxDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mHeap.ReleaseAndGetAddressOf()));

#if defined(RELEASE)
            (void)mDescription.name;
#else
            mHeap->SetName(utility::toWideString(mDescription.name).c_str());
#endif

            mDescriptorSize = GfxDevice()->GetDescriptorHandleIncrementSize(heapDesc.Type);

            switch (mDescription.flag)
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

        DescriptorHeap::~DescriptorHeap()
        {
            mHeap = nullptr;
        }



        bool DescriptorHeap::validateHandle(const DescriptorHandle& dHandle) const
        {
            if (dHandle.getCPUPtr() < mHeapStart.getCPUPtr() ||
                dHandle.getCPUPtr() >= mHeapStart.getCPUPtr() + mDescription.maxDescriptors * mDescriptorSize)
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
    }
}
