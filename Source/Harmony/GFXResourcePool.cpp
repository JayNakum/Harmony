#include "GFXResourcePool.h"

#include "GFXManager.h"

#include "Log.h"

namespace graphics
{
    std::vector<CommandContext> ContextPool::msContextPool[CommandListType::CommandListTypeCount];
    std::queue<CommandContext*> ContextPool::msAvailableContexts[CommandListType::CommandListTypeCount];

    CommandContext& ContextPool::requestContext(CommandListType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msContextAllocationMutex);

        std::queue<CommandContext*>& availableQueue = ContextPool::msAvailableContexts[type];

        if (!availableQueue.empty())
        {
            CommandContext* context = availableQueue.front();
            availableQueue.pop();
            context->reset();

            return *context;
        }

        ContextPool::msContextPool[type].emplace_back(type);
        return msContextPool[type].back();
    }

    void ContextPool::freeContext(CommandContext& context)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msContextAllocationMutex);

        ContextPool::msAvailableContexts[context.mType].push(&context);
    }

    void ContextPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msContextAllocationMutex);

        for (unsigned int i = 0; i < CommandListType::CommandListTypeCount; ++i) // for each type
        {
            // clear the queue to prevent dangling pointers
            while (!msAvailableContexts[i].empty())
            {	
                msAvailableContexts[i].pop();
            }

            ContextPool::msContextPool[i].clear();
        }
    }

    ContextPool::~ContextPool()
    {
        destroyPool();
    }
}

namespace graphics
{
    using namespace dx12;

    CommandAllocatorPool::CommandAllocatorPool(CommandListType type)
        : CommandObject(type) {} // TODO: Cache in a couple allocators already and put them in available queue

    CommandAllocator& CommandAllocatorPool::requestCommandAllocator()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mCmdAllocatorMutex);

        updateAvailableQueue();

        std::queue<CommandAllocator*>& availableQueue = mAvailableCmdAllocators;

        if (!availableQueue.empty())
        {
            CommandAllocator* allocator = availableQueue.front();
            availableQueue.pop();
            return *allocator;
        }

        CommandAllocator::Description createDesc;
        createDesc.name = (mCmdAllocatorPool.size() + " type[" + std::to_string(mType) + "] command allocator");
        createDesc.type = mType;
        mCmdAllocatorPool.emplace_back(createDesc);

        return mCmdAllocatorPool.back();
    }

    void CommandAllocatorPool::discardCommandAllocator(const uint64_t completedFenceValue, CommandAllocator& usedAllocator)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mCmdAllocatorMutex);

        mUsedCmdAllocators.push(std::make_pair(completedFenceValue, &usedAllocator));
    }

    void CommandAllocatorPool::destroyPool()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mCmdAllocatorMutex);

        updateAvailableQueue();
        HASSERT(mUsedCmdAllocators.empty(), "Trying to destroy the pool while allocators are still in used.");

        // clear the queues to prevent dangling pointers
        while (!mAvailableCmdAllocators.empty())
        {
            mAvailableCmdAllocators.pop();
        }

        mCmdAllocatorPool.clear();
    }

    CommandAllocatorPool::~CommandAllocatorPool()
    {
        destroyPool();
    }

    void CommandAllocatorPool::updateAvailableQueue()
    {
        while (!mUsedCmdAllocators.empty() && GfxCommandManager().isFenceComplete(mUsedCmdAllocators.front().first))
        {
            mAvailableCmdAllocators.push(mUsedCmdAllocators.front().second);
            mUsedCmdAllocators.pop();
        }
    }
}

namespace graphics
{
    using namespace dx12;

    DescriptorHeap& DescriptorHeapPool::requestDescriptorHeap(const DescriptorHeapType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msHeapAllocationMutex);

        updateAvailableQueue(type);

        if (!msAvailableDescriptorHeaps[type].empty())
        {
            DescriptorHeap* descriptorHeap = msAvailableDescriptorHeaps[type].front();
            msAvailableDescriptorHeaps[type].pop();
            return *descriptorHeap;
        }

        DescriptorHeap::Description createDesc;
        createDesc.type = type;
        createDesc.name = (DescriptorHeapPool::msDescriptorHeapPool[type].size() + " shader visible descriptor heap");
        createDesc.maxDescriptors = msNumDescriptorsPerHeap;

        DescriptorHeapPool::msDescriptorHeapPool[type].emplace_back(createDesc);

        return DescriptorHeapPool::msDescriptorHeapPool[type].back();
    }

    void DescriptorHeapPool::discardDescriptorHeap(const DescriptorHeapType type, uint64_t completedFenceValue, dx12::DescriptorHeap& usedHeaps)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msHeapAllocationMutex);

        msUsedDescriptorHeaps[type].push(std::make_pair(completedFenceValue, &usedHeaps));
    }

    void DescriptorHeapPool::updateAvailableQueue(const DescriptorHeapType type)
    {
        while (!msUsedDescriptorHeaps[type].empty() && GfxCommandManager().isFenceComplete(msUsedDescriptorHeaps[type].front().first))
        {
            msAvailableDescriptorHeaps[type].push(msUsedDescriptorHeaps[type].front().second);
            msUsedDescriptorHeaps[type].pop();
        }
    }
}
