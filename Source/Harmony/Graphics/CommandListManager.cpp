#include "CommandListManager.h"

#include "Renderer.h"

namespace graphics
{
    CommandListManager::CommandListManager()
        : mGraphicsQueue(CommandListType::Direct)
    {}

    CommandListManager::~CommandListManager()
    {
        msCommandListPool.destroyPool(); // We need to explicitely call this because the msCommandListPool is static
    }

    void CommandListManager::createNewCommandList(const CommandListType type, dx12::CommandList*& outCommandList, dx12::CommandAllocator*& outCommandAllocator)
    {
        outCommandAllocator = &mGraphicsQueue.commandAllocatorPool.requestCommandAllocator();
        outCommandList = &(msCommandListPool.requestCommandList(*outCommandAllocator));
    }

    CommandListManager::Queue& CommandListManager::getQueue(const CommandListType type)
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
}
