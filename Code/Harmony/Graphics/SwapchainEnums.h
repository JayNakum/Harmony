#pragma once

#include "common.h"

namespace graphics
{
    enum class PresentMode
    {
        None,
        Windowed
        // TODO: Add headless support
    };

    inline static const std::string toString(const PresentMode e)
    {
        switch (e)
        {
        default:
            return "";
        case PresentMode::None:
            return "None";
        case PresentMode::Windowed:
            return "Windowed";
        }
    }
}
