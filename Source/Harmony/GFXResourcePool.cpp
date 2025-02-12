#include "GFXResourcePool.h"

#include "GFXManager.h"

#include "Log.h"

namespace graphics
{
    std::vector<CommandContext*> ContextPool::msContextPool[CommandListType::CommandListTypeCount];
    std::queue<CommandContext*> ContextPool::msAvailableContexts[CommandListType::CommandListTypeCount];

    CommandContext& ContextPool::requestContext(CommandListType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msContextAllocationMutex);

        std::queue<CommandContext*>& availableQueue = ContextPool::msAvailableContexts[type];

        CommandContext* context = nullptr;

        if (!availableQueue.empty())
        {
            context = availableQueue.front();
            availableQueue.pop();

            context->reset();
        }
        else
        {
            context = new CommandContext(type);
            ContextPool::msContextPool[type].emplace_back(context);
        }

        HASSERT(context != nullptr, "Failed to allocate a command context of type %d", type);
        HASSERT(context->mType == type, "Context allocated with incorrect type.");

        return *context;
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

            for (CommandContext* context : ContextPool::msContextPool[i])
            {
                delete context;
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
        : mType(type) {} // TODO: Cache in a couple allocators already and put them in available queue

    CommandAllocator& CommandAllocatorPool::requestCommandAllocator()
    {
        std::lock_guard<std::mutex> scoppedLockGuard(mCmdAllocatorMutex);

        updateAvailableQueue();

        std::queue<CommandAllocator*>& availableQueue = mAvailableCmdAllocators;

        CommandAllocator* allocator = nullptr;

        if (!availableQueue.empty())
        {
            allocator = availableQueue.front();
            availableQueue.pop();
        }
        else
        {
            CommandAllocator::Description createDesc;
            createDesc.name = (mCmdAllocatorPool.size() + " type[" + std::to_string(mType) + "] command allocator");
            createDesc.type = mType;
            allocator = new CommandAllocator(createDesc);
            mCmdAllocatorPool.emplace_back(allocator);
        }

        HASSERT(allocator != nullptr, "Failed to allocate a CommandAllocator of type %d", mType);
        HASSERT(allocator->mType == mType, "CommandAllocator allocated with incorrect type.");

        return *allocator;
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

        for (CommandAllocator* context : mCmdAllocatorPool)
        {
            delete context;
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
        std::lock_guard<std::mutex> scoppedLockGuard(msHeapAllocattionMutex);

        updateAvailableQueue(type);

        DescriptorHeap* descriptorHeap = nullptr;

        if (!msAvailableDescriptorHeaps[type].empty())
        {
            descriptorHeap = msAvailableDescriptorHeaps[type].front();
            msAvailableDescriptorHeaps[type].pop();
        }
        else
        {
            DescriptorHeap::Description createDesc;
            createDesc.type = type;
            createDesc.name = (DescriptorHeapPool::msDescriptorHeapPool[type].size() + " shader visible descriptor heap");
            createDesc.maxDescriptors = msNumDescriptorsPerHeap;

            descriptorHeap = new DescriptorHeap(createDesc);

            DescriptorHeapPool::msDescriptorHeapPool[type].emplace_back(descriptorHeap);
        }

        return *descriptorHeap;
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
