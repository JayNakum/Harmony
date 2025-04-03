#pragma once

#include "common.h"

namespace graphics
{
    enum class RootParameterType
    {
        DescriptorTable,
        Constants32,
        CBV,
        SRV,
        UAV
    };

    inline static const std::string toString(const RootParameterType e)
    {
        switch (e)
        {
        default:
            return "";
        case RootParameterType::DescriptorTable:
            return "DescriptorTable";
        case RootParameterType::Constants32:
            return "32Bit Constants";
        case RootParameterType::CBV:
            return "CBV";
        case RootParameterType::SRV:
            return "SRV";
        case RootParameterType::UAV:
            return "UAV";
        }
    }

    enum class DescriptorRangeType
    {
        CBV,
        SRV,
        UAV,
        Sampler
    };

    inline static const std::string toString(const DescriptorRangeType e)
    {
        switch (e)
        {
        default:
            return "";
        case DescriptorRangeType::CBV:
            return "CBV";
        case DescriptorRangeType::SRV:
            return "SRV";
        case DescriptorRangeType::UAV:
            return "UAV";
        case DescriptorRangeType::Sampler:
            return "Sampler";
        }
    }

    enum class ShaderVisibility
    {
        All,
        Vertex,
        Hull,
        Domain,
        Geometry,
        Pixel,
        Amplification,
        Mesh
    };

    inline static const std::string toString(const ShaderVisibility e)
    {
        switch (e)
        {
        default:
            return "";
        case ShaderVisibility::All:
            return "All";
        case ShaderVisibility::Vertex:
            return "Vertex";
        case ShaderVisibility::Hull:
            return "Hull";
        case ShaderVisibility::Domain:
            return "Domain";
        case ShaderVisibility::Geometry:
            return "Geometry";
        case ShaderVisibility::Pixel:
            return "Pixel";
        case ShaderVisibility::Amplification:
            return "Amplification";
        case ShaderVisibility::Mesh:
            return "Mesh";
        }
    }

    namespace sampler
    {
        enum class Filter
        {
            MinMagMipPoint, //MIN_MAG_MIP_POINT
            //MIN_MAG_POINT_MIP_LINEAR,
            //MIN_POINT_MAG_LINEAR_MIP_POINT,
            //MIN_POINT_MAG_MIP_LINEAR,
            //MIN_LINEAR_MAG_MIP_POINT,
            //MIN_LINEAR_MAG_POINT_MIP_LINEAR,
            //MIN_MAG_LINEAR_MIP_POINT,
            MinMagMipLinear, //MIN_MAG_MIP_LINEAR
            Anisotropic //ANISOTROPIC
            //COMPARISON_MIN_MAG_MIP_POINT,
            //COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
            //COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
            //COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
            //COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
            //COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
            //COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
            //COMPARISON_MIN_MAG_MIP_LINEAR,
            //COMPARISON_ANISOTROPIC,
            //MINIMUM_MIN_MAG_MIP_POINT,
            //MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
            //MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
            //MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
            //MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
            //MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
            //MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
            //MINIMUM_MIN_MAG_MIP_LINEAR,
            //MINIMUM_ANISOTROPIC,
            //MAXIMUM_MIN_MAG_MIP_POINT,
            //MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
            //MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
            //MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
            //MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
            //MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
            //MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
            //MAXIMUM_MIN_MAG_MIP_LINEAR,
            //MAXIMUM_ANISOTROPIC
            // TODO: Add other filters support as needed
        };
    }

    inline static const std::string toString(const sampler::Filter e)
    {
        switch (e)
        {
        default:
            return "";
        case sampler::Filter::MinMagMipPoint:
            return "MinMagMipPoint";
        case sampler::Filter::MinMagMipLinear:
            return "MinMagMipLinear";
        case sampler::Filter::Anisotropic:
            return "Anisotropic";
        }
    }

    namespace sampler
    {
        enum class TextureAddressMode
        {
            Wrap,
            Mirror,
            Clamp,
            Border,
            MirrorOnce
        };
    }

    inline static const std::string toString(const sampler::TextureAddressMode e)
    {
        switch (e)
        {
        default:
            return "";
        case sampler::TextureAddressMode::Wrap:
            return "Wrap";
        case sampler::TextureAddressMode::Mirror:
            return "Mirror";
        case sampler::TextureAddressMode::Clamp:
            return "Clamp";
        case sampler::TextureAddressMode::Border:
            return "Border";
        case sampler::TextureAddressMode::MirrorOnce:
            return "MirrorOnce";
        }
    }

    namespace sampler
    {
        enum class ComparisionFunction
        {
            None,
            Never,
            Less,
            Equal,
            LessEqual,
            Greater,
            NotEqual,
            GreaterEqual,
            Always
        };
    }

    inline static const std::string toString(const sampler::ComparisionFunction e)
    {
        switch (e)
        {
        default:
            return "";
        case sampler::ComparisionFunction::None:
            return "None";
        case sampler::ComparisionFunction::Never:
            return "Never";
        case sampler::ComparisionFunction::Less:
            return "Less";
        case sampler::ComparisionFunction::Equal:
            return "Equal";
        case sampler::ComparisionFunction::LessEqual:
            return "LessEqual";
        case sampler::ComparisionFunction::Greater:
            return "Greater";
        case sampler::ComparisionFunction::NotEqual:
            return "NotEqual";
        case sampler::ComparisionFunction::GreaterEqual:
            return "GreaterEqual";
        case sampler::ComparisionFunction::Always:
            return "Always";
        }
    }

    namespace sampler
    {
        enum class StaticBorderColor
        {
            TransparentBlack,
            OpaqueBlack,
            OpaqueWhite,
        };
    }

    inline static const std::string toString(const sampler::StaticBorderColor e)
    {
        switch (e)
        {
        default:
            return "";
        case sampler::StaticBorderColor::TransparentBlack:
            return "TransparentBlack";
        case sampler::StaticBorderColor::OpaqueBlack:
            return "OpaqueBlack";
        case sampler::StaticBorderColor::OpaqueWhite:
            return "OpaqueWhite";
        }
    }
}
