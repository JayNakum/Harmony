#include "CommandListManager.h"

#include "Renderer.h"

namespace graphics
{
    CommandListManager::CommandListManager()
        : mGraphicsQueue({"main graphics queue", CommandListType::Direct})
    {}

    dx12::CommandQueue& CommandListManager::getCommandQueue(const CommandListType type)
    {
        switch (type)
        {
        case CommandListType::Compute:
        case CommandListType::Copy:
        case CommandListType::Direct:
        default:
            return mGraphicsQueue;
        }
    }

    void CommandListManager::createNewCommandList(const CommandListType type, dx12::CommandList* outCommandList, dx12::CommandAllocator* outCommandAllocator)
    {
        outCommandAllocator = &(GfxCommandAllocatorManager().requestAllocator(type));
        outCommandList = &(mCommandListPool.requestCommandList(*outCommandAllocator));
    }

}
