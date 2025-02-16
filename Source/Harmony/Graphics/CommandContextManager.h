#pragma once

#include "common.h"

#include "CommandContextPool.h"

namespace graphics
{
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
