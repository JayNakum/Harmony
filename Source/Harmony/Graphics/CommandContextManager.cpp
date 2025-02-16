#include "CommandContextManager.h"

namespace graphics
{
    std::array<CommandContextPool, CommandListType::CommandListTypeCount> CommandContextManager::msCommandContextPool = {
        CommandContextPool(CommandListType::Direct),
        CommandContextPool(CommandListType::Compute),
        CommandContextPool(CommandListType::Copy)
    };

    CommandContextManager::~CommandContextManager()
    {
        for (CommandContextPool& pool : msCommandContextPool)
        {
            pool.destroyPool(); // We need to explicitely call this because the msCommandContextPool is static
        }
    }

    CommandContext& CommandContextManager::requestContext(const CommandListType type)
    {
        return msCommandContextPool[type].requestContext();
    }

    void CommandContextManager::discardContext(CommandContext& usedContext)
    {
        msCommandContextPool[usedContext.getCommandListType()].discardContext(usedContext);
    }
}
