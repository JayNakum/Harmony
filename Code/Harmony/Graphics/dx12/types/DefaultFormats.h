#pragma once

#include "common.h"
#include "directx.h"

#include "Format.h"

namespace // Keeping this in no namespace so that these are inaccessible outside this translation unit
{
    // Hash function for the defaultFormats map
    struct FormatHasher
    {
        std::size_t operator()(const graphics::Format & f) const
        {
            std::size_t h = std::hash<int>()(static_cast<int>(f.kind));
            if (f.kind == graphics::Format::Kind::Uncompressed)
            {
                h ^= std::hash<int>()(static_cast<int>(f.uncompressed.channels)) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
                h ^= std::hash<int>()(static_cast<int>(f.uncompressed.type)) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
                for (auto b : f.uncompressed.bits)
                    h ^= std::hash<int>()(b) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
                h ^= std::hash<int>()(static_cast<int>(f.uncompressed.isDepth)) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            }
            else
            { // Compressed
                h ^= std::hash<int>()(f.compressed.blockWidth) + 0x9e3779b97f4a7c15ULL;
                h ^= std::hash<int>()(f.compressed.blockHeight) + 0x9e3779b97f4a7c15ULL;
                h ^= std::hash<int>()(f.compressed.blockSizeBytes) + 0x9e3779b97f4a7c15ULL;
                h ^= std::hash<int>()(static_cast<int>(f.compressed.type)) + 0x9e3779b97f4a7c15ULL;
            }
            return h;
        }
    };

    // Equal function for the defaultFormats map
    struct FormatEqual
    {
        bool operator()(const graphics::Format& a, const graphics::Format& b) const
        {
            if (a.kind != b.kind)
                return false;
            if (a.kind == graphics::Format::Kind::Uncompressed)
            {
                return a.uncompressed.channels == b.uncompressed.channels &&
                    a.uncompressed.type == b.uncompressed.type &&
                    a.uncompressed.bits == b.uncompressed.bits;
            }
            else
            {
                return a.compressed.blockWidth == b.compressed.blockWidth &&
                    a.compressed.blockHeight == b.compressed.blockHeight &&
                    a.compressed.blockSizeBytes == b.compressed.blockSizeBytes &&
                    a.compressed.type == b.compressed.type;
            }
        }
    };
}

namespace graphics
{
    namespace dx12
    {
        static const std::unordered_map<Format, DXGI_FORMAT, FormatHasher, FormatEqual> defaultFormats = {
            { DefaultFormats::UNKNOWN, DXGI_FORMAT_UNKNOWN },

            { DefaultFormats::R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_TYPELESS },
            { DefaultFormats::R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT },
            { DefaultFormats::R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_UINT },
            { DefaultFormats::R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_SINT },

            { DefaultFormats::R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32_TYPELESS },
            { DefaultFormats::R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT },
            { DefaultFormats::R32G32B32_UINT, DXGI_FORMAT_R32G32B32_UINT },
            { DefaultFormats::R32G32B32_SINT, DXGI_FORMAT_R32G32B32_SINT },

            { DefaultFormats::R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_TYPELESS },
            { DefaultFormats::R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT },
            { DefaultFormats::R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM },
            { DefaultFormats::R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_UINT },
            { DefaultFormats::R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM },
            { DefaultFormats::R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_SINT },

            { DefaultFormats::R32G32_TYPELESS, DXGI_FORMAT_R32G32_TYPELESS },
            { DefaultFormats::R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT },
            { DefaultFormats::R32G32_UINT, DXGI_FORMAT_R32G32_UINT },
            { DefaultFormats::R32G32_SINT, DXGI_FORMAT_R32G32_SINT },

            { DefaultFormats::R32G8X24_TYPELESS, DXGI_FORMAT_R32G8X24_TYPELESS },
            { DefaultFormats::D32_FLOAT_S8X24_UINT,  DXGI_FORMAT_D32_FLOAT_S8X24_UINT },
            { DefaultFormats::R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS },
            { DefaultFormats::X32_TYPELESS_G8X24_UINT, DXGI_FORMAT_X32_TYPELESS_G8X24_UINT },

            { DefaultFormats::R10G10B10A2_TYPELESS,  DXGI_FORMAT_R10G10B10A2_TYPELESS },
            { DefaultFormats::R10G10B10A2_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM },
            { DefaultFormats::R10G10B10A2_UINT, DXGI_FORMAT_R10G10B10A2_UINT },

            { DefaultFormats::R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT },

            { DefaultFormats::R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_TYPELESS },
            { DefaultFormats::R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM },
            { DefaultFormats::R8G8B8A8_UNORM_SRGB,   DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },
            { DefaultFormats::R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_UINT },
            { DefaultFormats::R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM },
            { DefaultFormats::R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_SINT },

            { DefaultFormats::R16G16_TYPELESS, DXGI_FORMAT_R16G16_TYPELESS },
            { DefaultFormats::R16G16_FLOAT, DXGI_FORMAT_R16G16_FLOAT },
            { DefaultFormats::R16G16_UNORM, DXGI_FORMAT_R16G16_UNORM },
            { DefaultFormats::R16G16_UINT, DXGI_FORMAT_R16G16_UINT },
            { DefaultFormats::R16G16_SNORM, DXGI_FORMAT_R16G16_SNORM },
            { DefaultFormats::R16G16_SINT, DXGI_FORMAT_R16G16_SINT },

            { DefaultFormats::R32_TYPELESS, DXGI_FORMAT_R32_TYPELESS },
            { DefaultFormats::D32_FLOAT, DXGI_FORMAT_D32_FLOAT },
            { DefaultFormats::R32_FLOAT, DXGI_FORMAT_R32_FLOAT },
            { DefaultFormats::R32_UINT, DXGI_FORMAT_R32_UINT },
            { DefaultFormats::R32_SINT, DXGI_FORMAT_R32_SINT },

            { DefaultFormats::R24G8_TYPELESS, DXGI_FORMAT_R24G8_TYPELESS },
            { DefaultFormats::D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT },
            { DefaultFormats::R24_UNORM_X8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
            { DefaultFormats::X24_TYPELESS_G8_UINT,  DXGI_FORMAT_X24_TYPELESS_G8_UINT },

            { DefaultFormats::R8G8_TYPELESS, DXGI_FORMAT_R8G8_TYPELESS },
            { DefaultFormats::R8G8_UNORM, DXGI_FORMAT_R8G8_UNORM },
            { DefaultFormats::R8G8_UINT, DXGI_FORMAT_R8G8_UINT },
            { DefaultFormats::R8G8_SNORM, DXGI_FORMAT_R8G8_SNORM },
            { DefaultFormats::R8G8_SINT, DXGI_FORMAT_R8G8_SINT },

            { DefaultFormats::R16_TYPELESS, DXGI_FORMAT_R16_TYPELESS },
            { DefaultFormats::R16_FLOAT, DXGI_FORMAT_R16_FLOAT },
            { DefaultFormats::D16_UNORM, DXGI_FORMAT_D16_UNORM },
            { DefaultFormats::R16_UNORM, DXGI_FORMAT_R16_UNORM },
            { DefaultFormats::R16_UINT, DXGI_FORMAT_R16_UINT },
            { DefaultFormats::R16_SNORM, DXGI_FORMAT_R16_SNORM },
            { DefaultFormats::R16_SINT, DXGI_FORMAT_R16_SINT },

            { DefaultFormats::R8_TYPELESS, DXGI_FORMAT_R8_TYPELESS },
            { DefaultFormats::R8_UNORM, DXGI_FORMAT_R8_UNORM },
            { DefaultFormats::R8_UINT, DXGI_FORMAT_R8_UINT },
            { DefaultFormats::R8_SNORM, DXGI_FORMAT_R8_SNORM },
            { DefaultFormats::R8_SINT, DXGI_FORMAT_R8_SINT },

            { DefaultFormats::A8_UNORM, DXGI_FORMAT_A8_UNORM },
            { DefaultFormats::R1_UNORM, DXGI_FORMAT_R1_UNORM },

            { DefaultFormats::R9G9B9E5_SHAREDEXP, DXGI_FORMAT_R9G9B9E5_SHAREDEXP },
            { DefaultFormats::R8G8_B8G8_UNORM, DXGI_FORMAT_R8G8_B8G8_UNORM },
            { DefaultFormats::G8R8_G8B8_UNORM, DXGI_FORMAT_G8R8_G8B8_UNORM },

            { DefaultFormats::BC1_TYPELESS, DXGI_FORMAT_BC1_TYPELESS },
            { DefaultFormats::BC1_UNORM, DXGI_FORMAT_BC1_UNORM },
            { DefaultFormats::BC1_UNORM_SRGB, DXGI_FORMAT_BC1_UNORM_SRGB },

            { DefaultFormats::BC2_TYPELESS, DXGI_FORMAT_BC2_TYPELESS },
            { DefaultFormats::BC2_UNORM, DXGI_FORMAT_BC2_UNORM },
            { DefaultFormats::BC2_UNORM_SRGB, DXGI_FORMAT_BC2_UNORM_SRGB },

            { DefaultFormats::BC3_TYPELESS, DXGI_FORMAT_BC3_TYPELESS },
            { DefaultFormats::BC3_UNORM, DXGI_FORMAT_BC3_UNORM },
            { DefaultFormats::BC3_UNORM_SRGB, DXGI_FORMAT_BC3_UNORM_SRGB },

            { DefaultFormats::BC4_TYPELESS, DXGI_FORMAT_BC4_TYPELESS },
            { DefaultFormats::BC4_UNORM, DXGI_FORMAT_BC4_UNORM },
            { DefaultFormats::BC4_SNORM, DXGI_FORMAT_BC4_SNORM },

            { DefaultFormats::BC5_TYPELESS, DXGI_FORMAT_BC5_TYPELESS },
            { DefaultFormats::BC5_UNORM, DXGI_FORMAT_BC5_UNORM },
            { DefaultFormats::BC5_SNORM, DXGI_FORMAT_BC5_SNORM },

            { DefaultFormats::BC6H_TYPELESS, DXGI_FORMAT_BC6H_TYPELESS },
            { DefaultFormats::BC6H_UF16, DXGI_FORMAT_BC6H_UF16 },
            { DefaultFormats::BC6H_SF16, DXGI_FORMAT_BC6H_SF16 },

            { DefaultFormats::BC7_TYPELESS, DXGI_FORMAT_BC7_TYPELESS },
            { DefaultFormats::BC7_UNORM, DXGI_FORMAT_BC7_UNORM },
            { DefaultFormats::BC7_UNORM_SRGB, DXGI_FORMAT_BC7_UNORM_SRGB },

            { DefaultFormats::B5G6R5_UNORM, DXGI_FORMAT_B5G6R5_UNORM },
            { DefaultFormats::B5G5R5A1_UNORM, DXGI_FORMAT_B5G5R5A1_UNORM },
            { DefaultFormats::B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM },
            { DefaultFormats::B8G8R8X8_UNORM, DXGI_FORMAT_B8G8R8X8_UNORM },
            { DefaultFormats::R10G10B10_XR_BIAS_A2_UNORM, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
            { DefaultFormats::B8G8R8A8_TYPELESS, DXGI_FORMAT_B8G8R8A8_TYPELESS },
            { DefaultFormats::B8G8R8A8_UNORM_SRGB,   DXGI_FORMAT_B8G8R8A8_UNORM_SRGB },
            { DefaultFormats::B8G8R8X8_TYPELESS, DXGI_FORMAT_B8G8R8X8_TYPELESS },
            { DefaultFormats::B8G8R8X8_UNORM_SRGB,   DXGI_FORMAT_B8G8R8X8_UNORM_SRGB },

            { DefaultFormats::AYUV, DXGI_FORMAT_AYUV },
            { DefaultFormats::Y410, DXGI_FORMAT_Y410 },
            { DefaultFormats::Y416, DXGI_FORMAT_Y416 },
        };

        // Maps graphics::Format with DXGI_FORMAT
        inline static const DXGI_FORMAT toDXGIFormat(const Format& format)
        {
            return defaultFormats.at(format);
        }
    }
}
