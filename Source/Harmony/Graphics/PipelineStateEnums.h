#pragma once

#include "common.h"

namespace graphics
{
    enum class Blend
    {
        Zero,
        One,
        SourceColor,
        InverseSourceColor,
        SourceAlpha,
        InverseSourceAlpha,
        DestinationAlpha,
        InverseDestinationAlpha,
        DestinationColor,
        InverseDestinationColor,
        SourceAlphaSaturate,
        BlendFactor,
        InverseBlendFactor,
        Source1Color,
        InverseSource1Color,
        Source1Alpha,
        InverseSource1Alpha,
        AlphaFactor,
        InverseAlphaFactor,
    };

    inline static const std::string toString(const Blend e)
    {
        switch (e)
        {
        default:
            return "";
        case Blend::Zero:
            return "Zero";
        case Blend::One:
            return "One";
        case Blend::SourceColor:
            return "SourceColor";
        case Blend::InverseSourceColor:
            return "InverseSourceColor";
        case Blend::SourceAlpha:
            return "SourceAlpha";
        case Blend::InverseSourceAlpha:
            return "InverseSourceAlpha";
        case Blend::DestinationAlpha:
            return "DestinationAlpha";
        case Blend::InverseDestinationAlpha:
            return "InverseDestinationAlpha";
        case Blend::DestinationColor:
            return "DestinationColor";
        case Blend::InverseDestinationColor:
            return "InverseDestinationColor";
        case Blend::SourceAlphaSaturate:
            return "SourceAlphaSaturate";
        case Blend::BlendFactor:
            return "BlendFactor";
        case Blend::InverseBlendFactor:
            return "InverseBlendFactor";
        case Blend::Source1Color:
            return "Source1Color";
        case Blend::InverseSource1Color:
            return "InverseSource1Color";
        case Blend::Source1Alpha:
            return "Source1Alpha";
        case Blend::InverseSource1Alpha:
            return "InverseSource1Alpha";
        case Blend::AlphaFactor:
            return "AlphaFactor";
        case Blend::InverseAlphaFactor:
            return "InverseAlphaFactor";
        }
    }

    enum class BlendOperator
    {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    inline static const std::string toString(const BlendOperator e)
    {
        switch (e)
        {
        default:
            return "";
        case BlendOperator::Add:
            return "Add";
        case BlendOperator::Subtract:
            return "Subtract";
        case BlendOperator::ReverseSubtract:
            return "ReverseSubtract";
        case BlendOperator::Min:
            return "Min";
        case BlendOperator::Max:
            return "Max";
        }
    }

    enum class BlendLogicOperator
    {
        Clear,
        Set,
        Copy,
        CopyInverted,
        NOOP,
        Invert,
        AND,
        NAND,
        OR,
        NOR,
        XOR,
        EQUIV,
        ANDReverse,
        ANDInverted,
        ORReverse,
        ORInverted
    };

    inline static const std::string toString(const BlendLogicOperator e)
    {
        switch (e)
        {
        default:
            return "";
        case BlendLogicOperator::Clear:
            return "Clear";
        case BlendLogicOperator::Set:
            return "Set";
        case BlendLogicOperator::Copy:
            return "Copy";
        case BlendLogicOperator::CopyInverted:
            return "CopyInverted";
        case BlendLogicOperator::NOOP:
            return "NOOP";
        case BlendLogicOperator::Invert:
            return "Invert";
        case BlendLogicOperator::AND:
            return "AND";
        case BlendLogicOperator::NAND:
            return "NAND";
        case BlendLogicOperator::OR:
            return "OR";
        case BlendLogicOperator::NOR:
            return "NOR";
        case BlendLogicOperator::XOR:
            return "XOR";
        case BlendLogicOperator::EQUIV:
            return "EQUIV";
        case BlendLogicOperator::ANDReverse:
            return "ANDReverse";
        case BlendLogicOperator::ANDInverted:
            return "ANDInverted";
        case BlendLogicOperator::ORReverse:
            return "ORReverse";
        case BlendLogicOperator::ORInverted:
            return "ORInverted";
        }
    }

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
