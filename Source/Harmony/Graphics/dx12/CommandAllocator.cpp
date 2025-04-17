#include "CommandAllocator.h"

#include "types/CommandListType.h"

#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        CommandAllocator::CommandAllocator(Device& device, const Description & createDescription)
            : CommandObject(createDescription.type), mCommandAllocator(nullptr)
        {
            HRESULT hResult = device->CreateCommandAllocator(toD3DCommandListType(mType), IID_PPV_ARGS(&mCommandAllocator));
            HASSERT(SUCCEEDED(hResult), "Failed to create command allocator %s.\n%s", createDescription.name, utility::translateErrorCode(hResult));

            setName(mCommandAllocator.Get(), utility::toWideString(createDescription.name).c_str());
        }

        CommandAllocator::~CommandAllocator()
        {
            mCommandAllocator = nullptr;
        }
    }
}
