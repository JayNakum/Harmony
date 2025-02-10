#include "GFXCommandList.h"

#include "GFXManager.h"

#include "Utility.h"
#include "Log.h"

namespace directx
{
    static const std::unordered_map<graphics::CommandListType, D3D12_COMMAND_LIST_TYPE> commandListType = {
        { graphics::CommandListType::Direct, D3D12_COMMAND_LIST_TYPE_DIRECT },
        { graphics::CommandListType::Compute, D3D12_COMMAND_LIST_TYPE_COMPUTE }
    };
}

namespace graphics
{
    namespace dx12
    {
        CommandAllocator::CommandAllocator(const Description & createDescription)
            : mType(createDescription.type)
        {
            HRESULT hResult = GfxDevice()->CreateCommandAllocator(directx::commandListType.at(mType), IID_PPV_ARGS(&mCommandAllocator));
            HASSERT_HR(hResult, "Failed to create command allocator %s.", createDescription.name);
#if defined(RELEASE)
            (void)createDescription.name;
#else
            mCommandAllocator->SetName(utility::toWideString(createDescription.name).c_str());
#endif
        }

        CommandAllocator::~CommandAllocator()
        {
            mCommandAllocator = nullptr;
        }
    }

    namespace dx12
    {

        CommandQueue::CommandQueue(const Description& createDescription)
            : mType(createDescription.type)
        {
            D3D12_COMMAND_QUEUE_DESC createDesc{};
            createDesc.Type = directx::commandListType.at(mType);
            createDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

            HRESULT hResult = GfxDevice()->CreateCommandQueue(&createDesc, IID_PPV_ARGS(&mCommandQueue));
            HASSERT_HR(hResult, "Failed to create command queue %s.", createDescription.name);

#if defined(RELEASE)
            (void)createDescription.name;
#else
            mCommandQueue->SetName(utility::toWideString(createDescription.name).c_str());
#endif
        }

        CommandQueue::~CommandQueue()
        {
            mCommandQueue = nullptr;
        }
    }
}
