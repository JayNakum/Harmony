#include "GFXResourcePool.h"

#include "GFXManager.h"

#include "Log.h"

namespace graphics
{
    std::vector<CommandContext*> ContextPool::msContextPool[CommandListType::CommandListTypeCount];
    std::queue<CommandContext*> ContextPool::msAvailableContexts[CommandListType::CommandListTypeCount];

    CommandContext& ContextPool::allocateContext(CommandListType type)
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

    void ContextPool::destroyAllContexts()
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
}

namespace graphics
{

    dx12::DescriptorHeap& DescriptorHeapPool::requestDescriptorHeap(DescriptorHeapType type)
    {
        std::lock_guard<std::mutex> scoppedLockGuard(msHeapAllocattionMutex);

        while (!msUsedDescriptorHeaps[type].empty() && GfxCommandManager().isFenceComplete(msUsedDescriptorHeaps[type].front().first))
        {
            msAvailableDescriptorHeaps[type].push(msUsedDescriptorHeaps[type].front().second);
            msUsedDescriptorHeaps[type].pop();
        }

        dx12::DescriptorHeap* descriptorHeap = nullptr;

        if (!msAvailableDescriptorHeaps[type].empty())
        {
            descriptorHeap = msAvailableDescriptorHeaps[type].front();
            msAvailableDescriptorHeaps[type].pop();
        }
        else
        {

            dx12::DescriptorHeap::Description createDesc;
            createDesc.type = type;
            createDesc.name = ("shader visible descriptor heap" + DescriptorHeapPool::msDescriptorHeapPool[type].size());
            createDesc.maxDescriptors = msNumDescriptorsPerHeap;

            descriptorHeap = new dx12::DescriptorHeap(createDesc);

            DescriptorHeapPool::msDescriptorHeapPool[type].emplace_back(descriptorHeap);
        }

        return *descriptorHeap;
    }
}
