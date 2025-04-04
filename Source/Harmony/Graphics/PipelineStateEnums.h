#pragma once

#include "common.h"

namespace graphics
{
    enum class FillMode
    {
        Solid,
        Wireframe
    };

    inline static const std::string toString(const FillMode e)
    {
        switch (e)
        {
        default:
            return "";
        case FillMode::Solid:
            return "Solid";
        case FillMode::Wireframe:
            return "Wireframe";
        }
    }


    enum class CullMode
    {
        None,
        Front,
        Back
    };

    inline static const std::string toString(const CullMode e)
    {
        switch (e)
        {
        default:
            "";
        case CullMode::None:
            return "None";
        case CullMode::Front:
            return "Front";
        case CullMode::Back:
            return "Back";
        }
    }
}
