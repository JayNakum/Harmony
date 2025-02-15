#include "CommandAllocator.h"

#include "Renderer.h"

#include "CommandListType.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        CommandAllocator::CommandAllocator(const Description & createDescription)
            : CommandObject(createDescription.type), mCommandAllocator(nullptr)
        {
            HRESULT hResult = GfxDevice()->CreateCommandAllocator(commandListType.at(mType), IID_PPV_ARGS(&mCommandAllocator));
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
}
