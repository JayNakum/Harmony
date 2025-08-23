#include "CommandAllocator.h"

#include "types/CommandListType.h"

namespace graphics
{
    CommandAllocator::CommandAllocator(Device& device, const Description & createDescription)
        : CommandObject(createDescription.type), mCommandAllocator(nullptr)
    {
        HRESULT hResult = device->CreateCommandAllocator(dx12::toD3DCommandListType(mType), IID_PPV_ARGS(&mCommandAllocator));
        HASSERT(SUCCEEDED(hResult), "Failed to create command allocator %s.\n%s", createDescription.name.c_str(), utility::translateErrorCode(hResult).c_str());

        setD3DName(mCommandAllocator.Get(), utility::toWideString(createDescription.name).c_str());
    }

    CommandAllocator::~CommandAllocator()
    {
        mCommandAllocator = nullptr;
    }
}
