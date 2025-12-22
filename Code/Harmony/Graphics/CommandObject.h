#pragma once

#include "common.h"

#include "CommandObjectEnums.h"

namespace graphics
{
    /*
    * Contains the common functions and variables that are needed by all the different command objects.
    */
    class CommandObject
    {
    protected:
        // Initializes the mType
        CommandObject(const CommandListType type)
            : mType(type) {}

        CommandObject(const CommandObject&) = delete;
        CommandObject& operator=(const CommandObject&) = delete;

        CommandObject(CommandObject&& move)
            : mType(move.mType) {}
        CommandObject& operator=(CommandObject&&) = delete;

    public:
        inline CommandListType getCommandListType() const { return mType; }

    protected:
        const CommandListType mType;
    };
}
