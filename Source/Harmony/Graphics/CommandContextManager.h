#pragma once

#include "common.h"

#include "CommandContextPool.h"

namespace graphics
{
    /*
    * A collection of all the type of CommandContext.
    * It is only ever meant to be used via the Renderer.
    */
    class CommandContextManager
    {
        friend class Renderer;
    private:
        CommandContextManager() = default;
        ~CommandContextManager();

    public:
        CommandContext& requestContext(const CommandListType type);
        void discardContext(CommandContext& usedContext);

    private:
        static std::array<CommandContextPool, CommandListType::CommandListTypeCount> msCommandContextPool;
    };
}
