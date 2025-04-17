#include "CommandListManager.h"

namespace graphics
{
    thread_local CommandListPool CommandListManager::msCommandListPool;
    CommandListManager::CommandListManager(dx12::Device& device)
        : mGraphicsQueue(device, CommandListType::Direct), mDevice(device)
    {}

    CommandListManager::~CommandListManager()
    {
        msCommandListPool.destroyPool(); // We need to explicitly call this because the msCommandListPool is static
        mGraphicsQueue.commandAllocatorPool.destroyPool();
        mGraphicsQueue.commandQueue->Release();
    }

    void CommandListManager::createNewCommandList(const CommandListType type, dx12::CommandList*& outCommandList, dx12::CommandAllocator*& outCommandAllocator)
    {
        outCommandAllocator = &(getQueue(type).requestAllocator(mDevice));
        outCommandList = &(msCommandListPool.requestCommandList(mDevice, type, *outCommandAllocator));
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
