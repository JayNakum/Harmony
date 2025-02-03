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
	CommandAllocator::CommandAllocator(const Description& createDescription)
		: mType(createDescription.type)
	{
		HRESULT hResult = GFXDevice->CreateCommandAllocator(directx::commandListType.at(mType), IID_PPV_ARGS(&mCommandAllocator));
		HASSERT_HR(hResult, "Failed to create command allocator.");
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