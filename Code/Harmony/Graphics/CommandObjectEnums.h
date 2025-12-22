#pragma once

#include "common.h"

namespace graphics
{
    enum class CommandListType
    {
        Direct,
        Compute,
        Copy
    };

    inline static std::string toString(const CommandListType e)
    {
        switch (e)
        {
        default:
            return "";
        case CommandListType::Direct:
            return "Direct";
        case CommandListType::Compute:
            return "Compute";
        case CommandListType::Copy:
            return "Copy";
        }
    }
}
