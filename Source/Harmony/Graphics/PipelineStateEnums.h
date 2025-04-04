#pragma once

#include "common.h"

namespace graphics
{
    enum class ColorWriteMask : uint8_t
    {
        None = (0 << 0),
        R = (1 << 0),
        G = (1 << 1),
        B = (1 << 2),
        A = (1 << 3),
        RG = R | G,
        RGB = RG | B,
        All = A | RGB
    };

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
