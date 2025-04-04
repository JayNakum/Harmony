#include "DescriptorHeap.h"

#include "Renderer.h"

#include "types/DescriptorHeapFlags.h"
#include "types/DescriptorHeapType.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        DescriptorHeap::DescriptorHeap(const Description & createDescription)
            : mDescription(createDescription), mHeap(nullptr), mHeapDesc{}, mDescriptorSize(0), mNumFreeDescriptors(0)
        {
            mHeapDesc.Type = descriptorHeapType.at(mDescription.type);
            mHeapDesc.NumDescriptors = (UINT)mDescription.maxDescriptors;
            mHeapDesc.Flags = descriptorHeapFlag.at(mDescription.flag);
            mHeapDesc.NodeMask = 1;

            HRESULT hResult = GfxDevice()->CreateDescriptorHeap(&mHeapDesc, IID_PPV_ARGS(mHeap.ReleaseAndGetAddressOf()));

#if defined(RELEASE)
            (void)mDescription.name;
#else
            mHeap->SetName(utility::toWideString(mDescription.name).c_str());
#endif

            mDescriptorSize = GfxDevice()->GetDescriptorHandleIncrementSize(mHeapDesc.Type);
            mNumFreeDescriptors = mHeapDesc.NumDescriptors;

            switch (mDescription.flag)
            {
            default:
            case DescriptorHeapFlags::GPUVisible:
            {
                mFirstHandle = DescriptorHandle(mHeap->GetCPUDescriptorHandleForHeapStart(),
                    mHeap->GetGPUDescriptorHandleForHeapStart());
                break;
            }
            case DescriptorHeapFlags::None:
            {
                mFirstHandle = DescriptorHandle(mHeap->GetCPUDescriptorHandleForHeapStart());
                break;
            }
            }

            mNextFreeHandle = mFirstHandle;
        }

        DescriptorHeap::~DescriptorHeap()
        {
            mHeap = nullptr;
        }

        DescriptorHandle DescriptorHeap::allocate(const size_t count)
        {
            HASSERT_SOFT(canAllocate(count), "%s Descriptor Heap out of space. Trying to allocate %d descriptors but available space is %d. Overwriting from the beginning.", mDescription.name, count, mNumFreeDescriptors);

            if (!canAllocate(count)) // Ring allocation
            {
                // It is possible that there is (some space < count) available but we do not care about it.
                // TODO: IF NEEDED implement a better allocation strategy.
                reset();
            }

            DescriptorHandle allocationHandle = mNextFreeHandle;

            mNextFreeHandle += (INT)(count * mDescriptorSize);
            mNumFreeDescriptors -= count;

            return allocationHandle;
        }

        void DescriptorHeap::reset()
        {
            mNumFreeDescriptors = mHeapDesc.NumDescriptors;
            mNextFreeHandle = mFirstHandle;
        }

        bool DescriptorHeap::validateHandle(const DescriptorHandle & dHandle) const
        {
            if (dHandle.getCPUPtr() < mFirstHandle.getCPUPtr() ||
                dHandle.getCPUPtr() >= mFirstHandle.getCPUPtr() + mHeapDesc.NumDescriptors * mDescriptorSize)
            {
                return false;
            }

            if (dHandle.isGPUVisible() != mFirstHandle.isGPUVisible())
            {
                return false;
            }

            if (dHandle.isGPUVisible())
            {
                if (dHandle.getGPUPtr() - mFirstHandle.getGPUPtr() !=
                    dHandle.getCPUPtr() - mFirstHandle.getCPUPtr())
                {
                    return false;
                }
            }

            return true;
        }
    }
}
