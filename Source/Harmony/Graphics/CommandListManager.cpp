//#include "CommandListManager.h"
//
//namespace graphics
//{
//    thread_local CommandListPool CommandListManager::msCommandListPool;
//    CommandListManager::CommandListManager(dx12::Device& device)
//        : mGraphicsQueue(device, CommandListType::Direct), mDevice(device)
//    {}
//
//    CommandListManager::~CommandListManager()
//    {
//        msCommandListPool.destroyPool(); // We need to explicitly call this because the msCommandListPool is static
//        mGraphicsQueue.commandAllocatorPool.destroyPool();
//    }
//
//    CommandListManager::CommandResources CommandListManager::createNewCommandList(const CommandListType type)
//    {
//        dx12::CommandAllocator newCommandAllocator = (getQueue(type).requestAllocator(mDevice));
//		dx12::CommandList newCommandList = (msCommandListPool.requestCommandList(mDevice, type, newCommandAllocator));
//
//        return { newCommandAllocator, newCommandList };
//    }
//
//    CommandListManager::Queue& CommandListManager::getQueue(const CommandListType type)
//    {
//        switch (type)
//        {
//        case CommandListType::Compute:
//        case CommandListType::Copy:
//        case CommandListType::Direct:
//        default:
//            return mGraphicsQueue;
//        }
//    }
//}
