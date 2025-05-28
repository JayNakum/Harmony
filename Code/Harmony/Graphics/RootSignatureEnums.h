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
        None = 0,
        Vertex = 1 << 0,
        Hull = 1 << 1,
        Domain = 1 << 2,
        Geometry = 1 << 3,
        Pixel = 1 << 4,
        Amplification = 1 << 5,
        Mesh = 1 << 6,
        All = Vertex | Hull | Domain | Geometry | Pixel | Amplification | Mesh
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

    enum class RootSignatureFlags
    {
        None = 0,
        AllowInputAssemblerInputLayout = 1 << 0,
        DenyVertexShaderRootAccess = 1 << 1,
        DenyHullShaderRootAccess = 1 << 2,
        DenyDomainShaderRootAccess = 1 << 3,
        DenyGeometryShaderRootAccess = 1 << 4,
        DenyPixelShaderRootAccess = 1 << 5,
        AllowStreamOutput = 1 << 6,
        LocalRootSignature = 1 << 7,
        DenyAmplificationShaderRootAccess = 1 << 8,
        DenyMeshShaderRootAccess = 1 << 9,
        ShaderViewHeapDirectlyIndexed = 1 << 10,
        SamplerHeapDirectlyIndexed = 1 << 11,
    };

    inline static const RootSignatureFlags operator|(RootSignatureFlags a, RootSignatureFlags b)
    {
        return static_cast<RootSignatureFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline static const RootSignatureFlags& operator|=(RootSignatureFlags& a, RootSignatureFlags b)
    {
        a = a | b;
        return a;
    }
    inline static const RootSignatureFlags operator&(RootSignatureFlags a, RootSignatureFlags b)
    {
        return static_cast<RootSignatureFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline static const bool hasFlag(RootSignatureFlags flags, RootSignatureFlags check)
    {
        return (flags & check) != RootSignatureFlags::None;
    }

    inline static const std::string toString(const RootSignatureFlags e)
    {
        switch (e)
        {
        default:
            return "";
        case RootSignatureFlags::None:
            return "None";
        case RootSignatureFlags::AllowInputAssemblerInputLayout:
            return "AllowInputAssemblerInputLayout";
        case RootSignatureFlags::DenyVertexShaderRootAccess:
            return "DenyVertexShaderRootAccess";
        case RootSignatureFlags::DenyHullShaderRootAccess:
            return "DenyHullShaderRootAccess";
        case RootSignatureFlags::DenyDomainShaderRootAccess:
            return "DenyDomainShaderRootAccess";
        case RootSignatureFlags::DenyGeometryShaderRootAccess:
            return "DenyGeometryShaderRootAccess";
        case RootSignatureFlags::DenyPixelShaderRootAccess:
            return "DenyPixelShaderRootAccess";
        case RootSignatureFlags::AllowStreamOutput:
            return "AllowStreamOutput";
        case RootSignatureFlags::LocalRootSignature:
            return "LocalRootSignature";
        case RootSignatureFlags::DenyAmplificationShaderRootAccess:
            return "DenyAmplificationShaderRootAccess";
        case RootSignatureFlags::DenyMeshShaderRootAccess:
            return "DenyMeshShaderRootAccess";
        case RootSignatureFlags::ShaderViewHeapDirectlyIndexed:
            return "ShaderViewHeapDirectlyIndexed";
        case RootSignatureFlags::SamplerHeapDirectlyIndexed:
            return "SamplerHeapDirectlyIndexed";
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
            OpaqueBlackUint,
            OpaqueWhiteUint
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
        case sampler::StaticBorderColor::OpaqueBlackUint:
            return "OpaqueBlackUint";
        case sampler::StaticBorderColor::OpaqueWhiteUint:
            return "OpaqueWhiteUint";
        }
    }
}
