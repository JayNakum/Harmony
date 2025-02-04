#include "GFXCommandContext.h"

#include "Log.h"

namespace graphics
{
    std::vector<CommandContext*> ContextPool::msContextPool[CommandListType::CommandListTypeCount];
    std::queue<CommandContext*> ContextPool::msAvailableContexts[CommandListType::CommandListTypeCount];

    CommandContext* ContextPool::allocateContext(CommandListType type)
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
        HASSERT(context->mType == type, "Context created with incorrect type.");

        return context;
    }

    void ContextPool::freeContext(CommandContext* context)
    {
        HASSERT(context != nullptr, "Trying to free a null context.");
        std::lock_guard<std::mutex> scoppedLockGuard(msContextAllocationMutex);

        ContextPool::msAvailableContexts[context->mType].push(context);
    }

    void ContextPool::destroyAllContexts()
    {
        for (unsigned int i = 0; i < CommandListType::CommandListTypeCount; ++i) // for each type
        {
            for (CommandContext* context : ContextPool::msContextPool[i])
            {
                delete context;
            }
            ContextPool::msContextPool[i].clear();
        }
    }

}

