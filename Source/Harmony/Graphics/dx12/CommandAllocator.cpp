#include "CommandAllocator.h"

#include "Core.h"

#include "types/CommandListType.h"

namespace graphics
{
    namespace dx12
    {
        CommandAllocator::CommandAllocator(const Description & createDescription)
            : CommandObject(createDescription.type), mCommandAllocator(nullptr)
        {
            HRESULT hResult = GfxDevice()->CreateCommandAllocator(toD3DCommandListType(mType), IID_PPV_ARGS(&mCommandAllocator));
            HASSERT_HR(hResult, "Failed to create command allocator %s.", createDescription.name);

            setName(mCommandAllocator.Get(), utility::toWideString(createDescription.name).c_str());
        }

        CommandAllocator::~CommandAllocator()
        {
            mCommandAllocator = nullptr;
        }
    }
}
