#include "CommandListManager.h"

#include "Renderer.h"

namespace graphics
{
    CommandListManager::CommandListManager()
        : mGraphicsQueue({"main graphics queue", CommandListType::Direct})
    {}

    CommandListManager::~CommandListManager()
    {
        msCommandListPool.destroyPool(); // We need to explicitely call this because the msCommandListPool is static
    }

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

    void CommandListManager::createNewCommandList(const CommandListType type, dx12::CommandList*& outCommandList, dx12::CommandAllocator*& outCommandAllocator)
    {
        outCommandAllocator = &(GfxCommandAllocatorManager().requestAllocator(type));
        outCommandList = &(msCommandListPool.requestCommandList(*outCommandAllocator));
    }

}
