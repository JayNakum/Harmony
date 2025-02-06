#include "GFXDescriptorHeap.h"

#include "GFXManager.h"

#include "Utility.h"
#include "Log.h"

namespace directx
{
    static const std::unordered_map<graphics::DescriptorHeapType, D3D12_DESCRIPTOR_HEAP_TYPE> descriptorHeapType = {
        { graphics::DescriptorHeapType::View, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV },
        { graphics::DescriptorHeapType::Sampler, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER },
        { graphics::DescriptorHeapType::RenderTarget, D3D12_DESCRIPTOR_HEAP_TYPE_RTV },
        { graphics::DescriptorHeapType::DepthStencil, D3D12_DESCRIPTOR_HEAP_TYPE_DSV }
    };

    static const std::unordered_map<graphics::DescriptorHeapFlag, D3D12_DESCRIPTOR_HEAP_FLAGS> descriptorHeapFlag = {
        { graphics::DescriptorHeapFlag::None, D3D12_DESCRIPTOR_HEAP_FLAG_NONE },
        { graphics::DescriptorHeapFlag::ShaderVisible, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE }
    };
}

namespace graphics
{
    DescriptorHeap::DescriptorHeap(const Description& createDescription)
    {
        mHeapDesc.Type = directx::descriptorHeapType.at(createDescription.type);
        mHeapDesc.NumDescriptors = createDescription.maxDescriptors;
        mHeapDesc.Flags = directx::descriptorHeapFlag.at(createDescription.flag);
        mHeapDesc.NodeMask = 1;

        HRESULT hResult = GfxDevice()->CreateDescriptorHeap(&mHeapDesc, IID_PPV_ARGS(mHeap.ReleaseAndGetAddressOf()));

#if defined(RELEASE)
        (void)createDescription.name;
#else
        mHeap->SetName(utility::toWideString(createDescription.name).c_str());
#endif

        mDescriptorSize = GfxDevice()->GetDescriptorHandleIncrementSize(mHeapDesc.Type);
        mNumFreeDescriptors = mHeapDesc.NumDescriptors;

        switch (createDescription.flag)
        {
        default:
        case DescriptorHeapFlag::ShaderVisible:
        {
            mFirstHandle = DescriptorHandle(mHeap->GetCPUDescriptorHandleForHeapStart(),
                                            mHeap->GetGPUDescriptorHandleForHeapStart());
            break;
        }
        case DescriptorHeapFlag::None:
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
        HASSERT(canAllocate(count), "Descriptor Heap out of space.");

        DescriptorHandle allocationHandle = mNextFreeHandle;

        mNextFreeHandle += count * mDescriptorSize;
        mNumFreeDescriptors -= count;

        return allocationHandle;
    }

    void DescriptorHeap::reset()
    {
        mNumFreeDescriptors = mHeapDesc.NumDescriptors;
        mNextFreeHandle = mFirstHandle;
    }

    bool DescriptorHeap::validateHandle(const DescriptorHandle& dHandle) const
    {
        if (dHandle.getCPUPtr() < mFirstHandle.getCPUPtr() ||
            dHandle.getCPUPtr() >= mFirstHandle.getCPUPtr() + mHeapDesc.NumDescriptors * mDescriptorSize)
        {
            return false;
        }

        if (dHandle.isShaderVisible() != mFirstHandle.isShaderVisible())
        {
            return false;
        }

        if (dHandle.isShaderVisible())
        {
            if (dHandle.getGPUPtr() - mFirstHandle.getGPUPtr() !=
                dHandle.getCPUPtr() - mFirstHandle.getCPUPtr())
            {
                return false;
            }
        }

        return true;
    }

    DescriptorHeap DescriptorHeapPool::requestDescriptorHeap(DescriptorHeapType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msHeapAllocattionMutex);

        while (!msUsedDescriptorHeaps[type].empty() && g_CommandManager.IsFenceComplete(msUsedDescriptorHeaps[type].front().first))
        {
            msAvailableDescriptorHeaps[type].push(msUsedDescriptorHeaps[type].front().second);
            msUsedDescriptorHeaps[type].pop();
        }

        if (!msAvailableDescriptorHeaps[type].empty())
        {
            DescriptorHeap* HeapPtr = msAvailableDescriptorHeaps[type].front();
            msAvailableDescriptorHeaps[type].pop();
            return HeapPtr;
        }
        else
        {
            D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
            HeapDesc.Type = HeapType;
            HeapDesc.NumDescriptors = kNumDescriptorsPerHeap;
            HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            HeapDesc.NodeMask = 1;
            Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> HeapPtr;
            ASSERT_SUCCEEDED(g_Device->CreateDescriptorHeap(&HeapDesc, MY_IID_PPV_ARGS(&HeapPtr)));
            sm_DescriptorHeapPool[type].emplace_back(HeapPtr);
            return HeapPtr.Get();
        }
    }
}
