#pragma once

#include "common.h"

namespace graphics
{
    // Defines which color channels are present.
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

    /*
    * The generic Format class that can represent either uncompressed or compressed formats.
    */
    class Format
    {
    public:
        // Specifies the data type of the format.
        enum class Type
        {
            Typeless,
            Float,
            Uint,
            Sint,
            Unorm
        };

        // Descriptor for uncompressed formats.
        struct Format::Uncompressed
        {
            ColorWriteMask channels; // Active color channels.
            Type type; // Numeric type.
            uint8_t bits[4]; // Bits per channel in order: {R, G, B, A}.
        };

        // Descriptor for compressed formats (example for block-compressed formats).
        struct Format::Compressed
        {
            uint8_t blockWidth; // Block width in pixels.
            uint8_t blockHeight; // Block height in pixels.
            uint8_t blockSizeBytes;  // Size of a block in bytes.
            Type type; // Data type, if applicable.
        };

        enum class Kind
        {
            Uncompressed,
            Compressed,
            // TODO: Extend with additional kinds if needed.
        };

        // Default constructor initializing to a default uncompressed format.
        Format()
            : kind(Kind::Uncompressed), uncompressed{ ColorWriteMask::None, Type::Typeless, {0, 0, 0, 0} }
        {}

        // Constructor for uncompressed formats.
        Format(const Format::Uncompressed& fmt)
            : kind(Kind::Uncompressed), uncompressed(fmt)
        {}

        // Constructor for compressed formats.
        Format(const Format::Compressed& fmt)
            : kind(Kind::Compressed), compressed(fmt)
        {}

        const Kind kind;
        union
        {
            const Format::Uncompressed uncompressed;
            const Format::Compressed compressed;
        };
    };

    struct DefaultFormats
    {
        // Uncompressed formats
        const Format UNKNOWN = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Typeless, {0, 0, 0, 0} });
        // R32G32B32A32 formats
        const Format R32G32B32A32_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {32, 32, 32, 32} });
        const Format R32G32B32A32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Float, {32, 32, 32, 32} });
        const Format R32G32B32A32_UINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Uint, {32, 32, 32, 32} });
        const Format R32G32B32A32_SINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Sint, {32, 32, 32, 32} });
        // R32G32B32 formats
        const Format R32G32B32_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Typeless, {32, 32, 32, 0} });
        const Format R32G32B32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Float, {32, 32, 32, 0} });
        const Format R32G32B32_UINT = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Uint, {32, 32, 32, 0} });
        const Format R32G32B32_SINT = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Sint, {32, 32, 32, 0} });
        // R16G16B16A16 formats
        const Format R16G16B16A16_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {16, 16, 16, 16} });
        const Format R16G16B16A16_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Float, {16, 16, 16, 16} });
        const Format R16G16B16A16_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {16, 16, 16, 16} });
        const Format R16G16B16A16_UINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Uint, {16, 16, 16, 16} });
        const Format R16G16B16A16_SNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {16, 16, 16, 16} });
        const Format R16G16B16A16_SINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Sint, {16, 16, 16, 16} });
        // R32G32 formats
        const Format R32G32_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {32, 32, 0, 0} });
        const Format R32G32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Float, {32, 32, 0, 0} });
        const Format R32G32_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Uint, {32, 32, 0, 0} });
        const Format R32G32_SINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Sint, {32, 32, 0, 0} });
        // Mixed depth/stencil formats
        const Format R32G8X24_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {32, 8, 0, 0} });
        const Format D32_FLOAT_S8X24_UINT = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Float, {32, 8, 0, 0} });
        const Format R32_FLOAT_X8X24_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Float, {32, 8, 0, 0} });
        const Format X32_TYPELESS_G8X24_UINT = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Typeless, {32, 8, 0, 0} });
        // R10G10B10A2 formats
        const Format R10G10B10A2_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {10, 10, 10, 2} });
        const Format R10G10B10A2_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {10, 10, 10, 2} });
        const Format R10G10B10A2_UINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Uint, {10, 10, 10, 2} });
        // R11G11B10_FLOAT
        const Format R11G11B10_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Float, {11, 11, 10, 0} });
        // R8G8B8A8 formats
        const Format R8G8B8A8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {8, 8, 8, 8} });
        const Format R8G8B8A8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        const Format R8G8B8A8_UNORM_SRGB = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        const Format R8G8B8A8_UINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Uint, {8, 8, 8, 8} });
        const Format R8G8B8A8_SNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        const Format R8G8B8A8_SINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Sint, {8, 8, 8, 8} });
        // R16G16 formats
        const Format R16G16_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {16, 16, 0, 0} });
        const Format R16G16_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Float, {16, 16, 0, 0} });
        const Format R16G16_UNORM = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Unorm, {16, 16, 0, 0} });
        const Format R16G16_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Uint, {16, 16, 0, 0} });
        const Format R16G16_SNORM = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Unorm, {16, 16, 0, 0} });
        const Format R16G16_SINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Sint, {16, 16, 0, 0} });
        // R32 formats
        const Format R32_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Typeless, {32, 0, 0, 0} });
        const Format D32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Float, {32, 0, 0, 0} });
        const Format R32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Float, {32, 0, 0, 0} });
        const Format R32_UINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Uint, {32, 0, 0, 0} });
        const Format R32_SINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Sint, {32, 0, 0, 0} });
        // Depth/stencil 24/8 formats
        const Format R24G8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Typeless, {24, 8, 0, 0} });
        const Format D24_UNORM_S8_UINT = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Unorm, {24, 8, 0, 0} });
        const Format R24_UNORM_X8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Unorm, {24, 8, 0, 0} });
        const Format X24_TYPELESS_G8_UINT = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Typeless, {24, 8, 0, 0} });
        // R8G8 formats
        const Format R8G8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {8, 8, 0, 0} });
        const Format R8G8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Unorm, {8, 8, 0, 0} });
        const Format R8G8_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Uint, {8, 8, 0, 0} });
        const Format R8G8_SNORM = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Unorm, {8, 8, 0, 0} });
        const Format R8G8_SINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Sint, {8, 8, 0, 0} });
        // R16 formats
        const Format R16_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Typeless, {16, 0, 0, 0} });
        const Format R16_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Float, {16, 0, 0, 0} });
        const Format D16_UNORM = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Unorm, {16, 0, 0, 0} });
        const Format R16_UNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {16, 0, 0, 0} });
        const Format R16_UINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Uint, {16, 0, 0, 0} });
        const Format R16_SNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {16, 0, 0, 0} });
        const Format R16_SINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Sint, {16, 0, 0, 0} });
        // R8 formats
        const Format R8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Typeless, {8, 0, 0, 0} });
        const Format R8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {8, 0, 0, 0} });
        const Format R8_UINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Uint, {8, 0, 0, 0} });
        const Format R8_SNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {8, 0, 0, 0} });
        const Format R8_SINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Sint, {8, 0, 0, 0} });
        // Alpha-only format
        const Format A8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::A, Format::Type::Unorm, {0, 0, 0, 8} });
        // 1-bit format
        const Format R1_UNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {1, 0, 0, 0} });
        // Special packed/float formats
        const Format R9G9B9E5_SHAREDEXP = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Float, {9, 9, 9, 5} });
        const Format R8G8_B8G8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        const Format G8R8_G8B8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        // Block compressed formats (using Format::Compressed descriptor)
        const Format BC1_TYPELESS = Format(Format::Compressed{ 4, 4, 8, Format::Type::Typeless });
        const Format BC1_UNORM = Format(Format::Compressed{ 4, 4, 8, Format::Type::Unorm });
        const Format BC1_UNORM_SRGB = Format(Format::Compressed{ 4, 4, 8, Format::Type::Unorm });
        const Format BC2_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        const Format BC2_UNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        const Format BC2_UNORM_SRGB = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        const Format BC3_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        const Format BC3_UNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        const Format BC3_UNORM_SRGB = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        const Format BC4_TYPELESS = Format(Format::Compressed{ 4, 4, 8, Format::Type::Typeless });
        const Format BC4_UNORM = Format(Format::Compressed{ 4, 4, 8, Format::Type::Unorm });
        const Format BC4_SNORM = Format(Format::Compressed{ 4, 4, 8, Format::Type::Unorm });
        const Format BC5_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        const Format BC5_UNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        const Format BC5_SNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        const Format BC6H_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        const Format BC6H_UF16 = Format(Format::Compressed{ 4, 4, 16, Format::Type::Float });
        const Format BC6H_SF16 = Format(Format::Compressed{ 4, 4, 16, Format::Type::Float });
        const Format BC7_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        const Format BC7_UNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        const Format BC7_UNORM_SRGB = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        // BGR/BGRA formats with different channel ordering
        const Format B5G6R5_UNORM = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Unorm, {5, 6, 5, 0} });
        const Format B5G5R5A1_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {5, 5, 5, 1} });
        const Format B8G8R8A8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        const Format B8G8R8X8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Unorm, {8, 8, 8, 0} });
        const Format R10G10B10_XR_BIAS_A2_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {10, 10, 10, 2} });
        const Format B8G8R8A8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {8, 8, 8, 8} });
        const Format B8G8R8A8_UNORM_SRGB = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        const Format B8G8R8X8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Typeless, {8, 8, 8, 0} });
        const Format B8G8R8X8_UNORM_SRGB = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Unorm, {8, 8, 8, 0} });
        // Additional special formats
        const Format AYUV = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {8, 8, 8, 8} });
        const Format Y410 = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {10, 10, 10, 2} });
        const Format Y416 = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {16, 16, 16, 16} });
    };

}

//UNKNOWN,
//R32G32B32A32_TYPELESS,
//R32G32B32A32_FLOAT,
//R32G32B32A32_UINT,
//R32G32B32A32_SINT,
//R32G32B32_TYPELESS,
//R32G32B32_FLOAT,
//R32G32B32_UINT,
//R32G32B32_SINT,
//R16G16B16A16_TYPELESS,
//R16G16B16A16_FLOAT,
//R16G16B16A16_UNORM,
//R16G16B16A16_UINT,
//R16G16B16A16_SNORM,
//R16G16B16A16_SINT,
//R32G32_TYPELESS,
//R32G32_FLOAT,
//R32G32_UINT,
//R32G32_SINT,
//R32G8X24_TYPELESS,
//D32_FLOAT_S8X24_UINT,
//R32_FLOAT_X8X24_TYPELESS,
//X32_TYPELESS_G8X24_UINT,
//R10G10B10A2_TYPELESS,
//R10G10B10A2_UNORM,
//R10G10B10A2_UINT,
//R11G11B10_FLOAT,
//R8G8B8A8_TYPELESS,
//R8G8B8A8_UNORM,
//R8G8B8A8_UNORM_SRGB,
//R8G8B8A8_UINT,
//R8G8B8A8_SNORM,
//R8G8B8A8_SINT,
//R16G16_TYPELESS,
//R16G16_FLOAT,
//R16G16_UNORM,
//R16G16_UINT,
//R16G16_SNORM,
//R16G16_SINT,
//R32_TYPELESS,
//D32_FLOAT,
//R32_FLOAT,
//R32_UINT,
//R32_SINT,
//R24G8_TYPELESS,
//D24_UNORM_S8_UINT,
//R24_UNORM_X8_TYPELESS,
//X24_TYPELESS_G8_UINT,
//R8G8_TYPELESS,
//R8G8_UNORM,
//R8G8_UINT,
//R8G8_SNORM,
//R8G8_SINT,
//R16_TYPELESS,
//R16_FLOAT,
//D16_UNORM,
//R16_UNORM,
//R16_UINT,
//R16_SNORM,
//R16_SINT,
//R8_TYPELESS,
//R8_UNORM,
//R8_UINT,
//R8_SNORM,
//R8_SINT,
//A8_UNORM,
//R1_UNORM,
//R9G9B9E5_SHAREDEXP,
//R8G8_B8G8_UNORM,
//G8R8_G8B8_UNORM,
//BC1_TYPELESS,
//BC1_UNORM,
//BC1_UNORM_SRGB,
//BC2_TYPELESS,
//BC2_UNORM,
//BC2_UNORM_SRGB,
//BC3_TYPELESS,
//BC3_UNORM,
//BC3_UNORM_SRGB,
//BC4_TYPELESS,
//BC4_UNORM,
//BC4_SNORM,
//BC5_TYPELESS,
//BC5_UNORM,
//BC5_SNORM,
//B5G6R5_UNORM,
//B5G5R5A1_UNORM,
//B8G8R8A8_UNORM,
//B8G8R8X8_UNORM,
//R10G10B10_XR_BIAS_A2_UNORM,
//B8G8R8A8_TYPELESS,
//B8G8R8A8_UNORM_SRGB,
//B8G8R8X8_TYPELESS,
//B8G8R8X8_UNORM_SRGB,
//BC6H_TYPELESS,
//BC6H_UF16,
//BC6H_SF16,
//BC7_TYPELESS,
//BC7_UNORM,
//BC7_UNORM_SRGB,
//AYUV,
//Y410,
//Y416,
//NV12,
//P010,
//P016,
//OPAQUE_420,
//YUY2,
//Y210,
//Y216,
//NV11,
//AI44,
//IA44,
//P8,
//A8P8,
//B4G4R4A4_UNORM,
//
//P208,
//V208,
//V408,
//
//
//SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
//SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,
//
//A4B4G4R4_UNORM,
//
//
//FORCE_UINT,
