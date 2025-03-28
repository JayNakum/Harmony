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

    const std::string toString(const RootParameterType e)
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

    const std::string toString(const DescriptorRangeType e)
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
        // Hull,
        // Domain,
        // Geometry,
        Pixel
        // Amplification,
        // Mesh
        // TODO: Add other shaders support
    };

    const std::string toString(const ShaderVisibility e)
    {
        switch (e)
        {
        default:
            return "";
        case ShaderVisibility::All:
            return "All";
        case ShaderVisibility::Vertex:
            return "Vertex";
        /*case ShaderVisibility::Hull:
            return "Hull";
        case ShaderVisibility::Domain:
            return "Domain";
        case ShaderVisibility::Geometry:
            return "Geometry";*/
        case ShaderVisibility::Pixel:
            return "Pixel";
        /*case ShaderVisibility::Amplification:
            return "Amplification";
        case ShaderVisibility::Mesh:
            return "Mesh";*/
        // TODO: Add other shaders support
        }
    }
}
