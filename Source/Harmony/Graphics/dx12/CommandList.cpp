#include "CommandList.h"

#include "Renderer.h"

#include "CommandListType.h"

#include "Utility.h"
#include "Log.h"

namespace graphics
{
    namespace dx12
    {
        CommandList::CommandList(Description& createDesc)
            : CommandObject(createDesc.type), mCommandList(nullptr)
        {
            HRESULT hResult = GfxDevice()->CreateCommandList(
                1, commandListType.at(createDesc.type),
                (createDesc.allocator).get(), nullptr, IID_PPV_ARGS(&mCommandList));

            HASSERT_HR(hResult, "Failed to create command list %s", createDesc.name);

#if defined(RELEASE)
            (void)name;
#else
            mCommandList->SetName(utility::toWideString(createDesc.name).c_str());
#endif
        }

        CommandList::~CommandList()
        {
            HRESULT hResult = mCommandList->Close();
            HASSERT_HR(hResult, "Failed to close command list.");
            mCommandList = nullptr;
        }
    }
}
