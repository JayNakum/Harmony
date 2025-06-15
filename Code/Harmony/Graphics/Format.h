#pragma once

#include "common.h"
#include "Log.h"

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
            Unorm,
            Snorm
        };

        // Descriptor for uncompressed formats.
        struct Uncompressed
        {
            ColorWriteMask channels; // Active color channels.
            Type type; // Numeric type.
            std::array<uint8_t, 4> bits; // Bits per channel in order: {R, G, B, A}.
        };

        // Descriptor for compressed formats (example for block-compressed formats).
        struct Compressed
        {
            uint8_t blockWidth; // Block width in pixels.
            uint8_t blockHeight; // Block height in pixels.
            uint8_t blockSizeBytes;  // Size of a block in bytes.
            Type type; // Data type, if applicable.
        };

        enum class Kind
        {
            Uncompressed,
            Compressed
        };

    public:
        // Default constructor initializing to a default uncompressed format.
        Format()
            : kind(Kind::Uncompressed), uncompressed{ ColorWriteMask::None, Type::Typeless, {0, 0, 0, 0} }
        {}

        // Constructor for uncompressed formats.
        Format(const Uncompressed& fmt)
            : kind(Kind::Uncompressed), uncompressed(fmt)
        {}

        // Constructor for compressed formats.
        Format(const Compressed& fmt)
            : kind(Kind::Compressed), compressed(fmt)
        {}

    public:
        inline bool operator==(const Format& other) const
        {
            if (kind != other.kind)
                return false;

            if (kind == Kind::Uncompressed)
            {
                return (uncompressed.channels == other.uncompressed.channels) &&
                    (uncompressed.type == other.uncompressed.type) &&
                    (uncompressed.bits[0] == other.uncompressed.bits[0]) &&
                    (uncompressed.bits[1] == other.uncompressed.bits[1]) &&
                    (uncompressed.bits[2] == other.uncompressed.bits[2]) &&
                    (uncompressed.bits[3] == other.uncompressed.bits[3]);
            }
            else // Kind::Compressed
            {
                return (compressed.blockWidth == other.compressed.blockWidth) &&
                    (compressed.blockHeight == other.compressed.blockHeight) &&
                    (compressed.blockSizeBytes == other.compressed.blockSizeBytes) &&
                    (compressed.type == other.compressed.type);
            }
        }

        inline bool operator!=(const Format& other) const
        {
            return !(*this == other);
        }

    public:
        Kind kind;
        union
        {
            Uncompressed uncompressed;
            Compressed compressed;
        };
    };

    namespace DefaultFormats
    {
        static Format getBaseFormat(const Format& format);
        static Format getUAVFormat(const Format& format);
        static Format getDSVFormat(const Format& format);
        static Format getDepthFormat(const Format& format);
        static Format getStencilFormat(const Format& format);

        // For compressed formats the function returns the blockSizeBytes
        static size_t bytesPerPixel(const Format& format);

        // Uncompressed formats
        static const Format UNKNOWN = Format(Format::Uncompressed{ ColorWriteMask::None, Format::Type::Typeless, {0, 0, 0, 0} });
        // R32G32B32A32 formats
        static const Format R32G32B32A32_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {32, 32, 32, 32} });
        static const Format R32G32B32A32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Float, {32, 32, 32, 32} });
        static const Format R32G32B32A32_UINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Uint, {32, 32, 32, 32} });
        static const Format R32G32B32A32_SINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Sint, {32, 32, 32, 32} });
        // R32G32B32 formats
        static const Format R32G32B32_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Typeless, {32, 32, 32, 0} });
        static const Format R32G32B32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Float, {32, 32, 32, 0} });
        static const Format R32G32B32_UINT = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Uint, {32, 32, 32, 0} });
        static const Format R32G32B32_SINT = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Sint, {32, 32, 32, 0} });
        // R16G16B16A16 formats
        static const Format R16G16B16A16_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {16, 16, 16, 16} });
        static const Format R16G16B16A16_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Float, {16, 16, 16, 16} });
        static const Format R16G16B16A16_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {16, 16, 16, 16} });
        static const Format R16G16B16A16_UINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Uint, {16, 16, 16, 16} });
        static const Format R16G16B16A16_SNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Snorm, {16, 16, 16, 16} });
        static const Format R16G16B16A16_SINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Sint, {16, 16, 16, 16} });
        // R32G32 formats
        static const Format R32G32_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {32, 32, 0, 0} });
        static const Format R32G32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Float, {32, 32, 0, 0} });
        static const Format R32G32_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Uint, {32, 32, 0, 0} });
        static const Format R32G32_SINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Sint, {32, 32, 0, 0} });
        // Mixed depth/stencil formats
        static const Format R32G8X24_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {32, 8, 0, 0} });
        static const Format D32_FLOAT_S8X24_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Float, {32, 8, 0, 0} });
        static const Format R32_FLOAT_X8X24_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {32, 8, 0, 0} });
        static const Format X32_TYPELESS_G8X24_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Uint, {32, 8, 0, 0} });
        // R10G10B10A2 formats
        static const Format R10G10B10A2_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {10, 10, 10, 2} });
        static const Format R10G10B10A2_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {10, 10, 10, 2} });
        static const Format R10G10B10A2_UINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Uint, {10, 10, 10, 2} });
        // R11G11B10_FLOAT
        static const Format R11G11B10_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::RGB,  Format::Type::Float, {11, 11, 10, 0} });
        // R8G8B8A8 formats
        static const Format R8G8B8A8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {8, 8, 8, 8} });
        static const Format R8G8B8A8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        static const Format R8G8B8A8_UNORM_SRGB = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        static const Format R8G8B8A8_UINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Uint, {8, 8, 8, 8} });
        static const Format R8G8B8A8_SNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Snorm, {8, 8, 8, 8} });
        static const Format R8G8B8A8_SINT = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Sint, {8, 8, 8, 8} });
        // R16G16 formats
        static const Format R16G16_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {16, 16, 0, 0} });
        static const Format R16G16_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Float, {16, 16, 0, 0} });
        static const Format R16G16_UNORM = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Unorm, {16, 16, 0, 0} });
        static const Format R16G16_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Uint, {16, 16, 0, 0} });
        static const Format R16G16_SNORM = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Snorm, {16, 16, 0, 0} });
        static const Format R16G16_SINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Sint, {16, 16, 0, 0} });
        // R32 formats
        static const Format R32_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Typeless, {32, 0, 0, 0} });
        static const Format D32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Float, {32, 0, 0, 0} });
        static const Format R32_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Float, {32, 0, 0, 0} });
        static const Format R32_UINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Uint, {32, 0, 0, 0} });
        static const Format R32_SINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Sint, {32, 0, 0, 0} });
        // Depth/stencil 24/8 formats
        static const Format R24G8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {24, 8, 0, 0} });
        static const Format D24_UNORM_S8_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Unorm, {24, 8, 0, 0} });
        static const Format R24_UNORM_X8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Unorm, {24, 8, 0, 0} });
        static const Format X24_TYPELESS_G8_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {24, 8, 0, 0} });
        // R8G8 formats
        static const Format R8G8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Typeless, {8, 8, 0, 0} });
        static const Format R8G8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Unorm, {8, 8, 0, 0} });
        static const Format R8G8_UINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Uint, {8, 8, 0, 0} });
        static const Format R8G8_SNORM = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Snorm, {8, 8, 0, 0} });
        static const Format R8G8_SINT = Format(Format::Uncompressed{ ColorWriteMask::RG, Format::Type::Sint, {8, 8, 0, 0} });
        // R16 formats
        static const Format R16_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Typeless, {16, 0, 0, 0} });
        static const Format R16_FLOAT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Float, {16, 0, 0, 0} });
        static const Format D16_UNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {16, 0, 0, 0} });
        static const Format R16_UNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {16, 0, 0, 0} });
        static const Format R16_UINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Uint, {16, 0, 0, 0} });
        static const Format R16_SNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Snorm, {16, 0, 0, 0} });
        static const Format R16_SINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Sint, {16, 0, 0, 0} });
        // R8 formats
        static const Format R8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Typeless, {8, 0, 0, 0} });
        static const Format R8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {8, 0, 0, 0} });
        static const Format R8_UINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Uint, {8, 0, 0, 0} });
        static const Format R8_SNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Snorm, {8, 0, 0, 0} });
        static const Format R8_SINT = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Sint, {8, 0, 0, 0} });
        // Alpha-only format
        static const Format A8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::A, Format::Type::Unorm, {0, 0, 0, 8} });
        // 1-bit format
        static const Format R1_UNORM = Format(Format::Uncompressed{ ColorWriteMask::R, Format::Type::Unorm, {1, 0, 0, 0} });
        // Special packed/float formats
        static const Format R9G9B9E5_SHAREDEXP = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Float, {9, 9, 9, 5} });
        static const Format R8G8_B8G8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        static const Format G8R8_G8B8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        // Block compressed formats (using Format::Compressed descriptor)
        static const Format BC1_TYPELESS = Format(Format::Compressed{ 4, 4, 8, Format::Type::Typeless });
        static const Format BC1_UNORM = Format(Format::Compressed{ 4, 4, 8, Format::Type::Unorm });
        static const Format BC1_UNORM_SRGB = Format(Format::Compressed{ 4, 4, 8, Format::Type::Unorm });
        static const Format BC2_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        static const Format BC2_UNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        static const Format BC2_UNORM_SRGB = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        static const Format BC3_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        static const Format BC3_UNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        static const Format BC3_UNORM_SRGB = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        static const Format BC4_TYPELESS = Format(Format::Compressed{ 4, 4, 8, Format::Type::Typeless });
        static const Format BC4_UNORM = Format(Format::Compressed{ 4, 4, 8, Format::Type::Unorm });
        static const Format BC4_SNORM = Format(Format::Compressed{ 4, 4, 8, Format::Type::Snorm });
        static const Format BC5_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        static const Format BC5_UNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        static const Format BC5_SNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Snorm });
        static const Format BC6H_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        static const Format BC6H_UF16 = Format(Format::Compressed{ 4, 4, 16, Format::Type::Float });
        static const Format BC6H_SF16 = Format(Format::Compressed{ 4, 4, 16, Format::Type::Float });
        static const Format BC7_TYPELESS = Format(Format::Compressed{ 4, 4, 16, Format::Type::Typeless });
        static const Format BC7_UNORM = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        static const Format BC7_UNORM_SRGB = Format(Format::Compressed{ 4, 4, 16, Format::Type::Unorm });
        // BGR/BGRA formats with different channel ordering
        static const Format B5G6R5_UNORM = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Unorm, {5, 6, 5, 0} });
        static const Format B5G5R5A1_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {5, 5, 5, 1} });
        static const Format B8G8R8A8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        static const Format B8G8R8X8_UNORM = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Unorm, {8, 8, 8, 0} });
        static const Format R10G10B10_XR_BIAS_A2_UNORM = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {10, 10, 10, 2} });
        static const Format B8G8R8A8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {8, 8, 8, 8} });
        static const Format B8G8R8A8_UNORM_SRGB = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Unorm, {8, 8, 8, 8} });
        static const Format B8G8R8X8_TYPELESS = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Typeless, {8, 8, 8, 0} });
        static const Format B8G8R8X8_UNORM_SRGB = Format(Format::Uncompressed{ ColorWriteMask::RGB, Format::Type::Unorm, {8, 8, 8, 0} });
        // Additional special formats
        static const Format AYUV = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {8, 8, 8, 8} });
        static const Format Y410 = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {10, 10, 10, 2} });
        static const Format Y416 = Format(Format::Uncompressed{ ColorWriteMask::All, Format::Type::Typeless, {16, 16, 16, 16} });
    };
}
