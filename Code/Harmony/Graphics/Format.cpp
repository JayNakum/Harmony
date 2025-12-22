#include "Format.h"

namespace graphics
{
    Format DefaultFormats::getBaseFormat(const Format& format)
    {
        if (format == DefaultFormats::R8G8B8A8_UNORM ||
            format == DefaultFormats::R8G8B8A8_UNORM_SRGB)
        {
            return DefaultFormats::R8G8B8A8_TYPELESS;
        }
        else if (format == DefaultFormats::B8G8R8A8_UNORM ||
            format == DefaultFormats::B8G8R8A8_UNORM_SRGB)
        {
            return DefaultFormats::B8G8R8A8_TYPELESS;
        }
        else if (format == DefaultFormats::B8G8R8X8_UNORM ||
            format == DefaultFormats::B8G8R8X8_UNORM_SRGB)
        {
            return DefaultFormats::B8G8R8X8_TYPELESS;
        }
        else if (format == DefaultFormats::R32G8X24_TYPELESS ||
            format == DefaultFormats::D32_FLOAT_S8X24_UINT ||
            format == DefaultFormats::R32_FLOAT_X8X24_TYPELESS ||
            format == DefaultFormats::X32_TYPELESS_G8X24_UINT)
        {
            return DefaultFormats::R32G8X24_TYPELESS;
        }
        else if (format == DefaultFormats::R32_TYPELESS ||
            format == DefaultFormats::D32_FLOAT ||
            format == DefaultFormats::R32_FLOAT)
        {
            return DefaultFormats::R32_TYPELESS;
        }
        else if (format == DefaultFormats::R24G8_TYPELESS ||
            format == DefaultFormats::D24_UNORM_S8_UINT ||
            format == DefaultFormats::R24_UNORM_X8_TYPELESS ||
            format == DefaultFormats::X24_TYPELESS_G8_UINT)
        {
            return DefaultFormats::R24G8_TYPELESS;
        }
        else if (format == DefaultFormats::R16_TYPELESS ||
            format == DefaultFormats::D16_UNORM ||
            format == DefaultFormats::R16_UNORM)
        {
            return DefaultFormats::R16_TYPELESS;
        }
        else
        {
            return format;
        }
    }

    Format DefaultFormats::getUAVFormat(const Format& format)
    {
        if (format == DefaultFormats::R8G8B8A8_TYPELESS ||
            format == DefaultFormats::R8G8B8A8_UNORM ||
            format == DefaultFormats::R8G8B8A8_UNORM_SRGB)
        {
            return DefaultFormats::R8G8B8A8_UNORM;
        }
        else if (format == DefaultFormats::B8G8R8A8_TYPELESS ||
            format == DefaultFormats::B8G8R8A8_UNORM ||
            format == DefaultFormats::B8G8R8A8_UNORM_SRGB)
        {
            return DefaultFormats::B8G8R8A8_UNORM;
        }
        else if (format == DefaultFormats::B8G8R8X8_TYPELESS ||
            format == DefaultFormats::B8G8R8X8_UNORM ||
            format == DefaultFormats::B8G8R8X8_UNORM_SRGB)
        {
            return DefaultFormats::B8G8R8X8_UNORM;
        }
        else if (format == DefaultFormats::R32_TYPELESS ||
            format == DefaultFormats::R32_FLOAT)
        {
            return DefaultFormats::R32_FLOAT;
        }
#if !defined(HRELEASE)
        else if (format == DefaultFormats::R32G8X24_TYPELESS ||
            format == DefaultFormats::D32_FLOAT_S8X24_UINT ||
            format == DefaultFormats::R32_FLOAT_X8X24_TYPELESS ||
            format == DefaultFormats::X32_TYPELESS_G8X24_UINT ||
            format == DefaultFormats::D32_FLOAT ||
            format == DefaultFormats::R24G8_TYPELESS ||
            format == DefaultFormats::D24_UNORM_S8_UINT ||
            format == DefaultFormats::R24_UNORM_X8_TYPELESS ||
            format == DefaultFormats::X24_TYPELESS_G8_UINT ||
            format == DefaultFormats::D16_UNORM)
        {
            HASSERT(false, "Requested a UAV Format for a depth stencil Format.");
        }
#endif
        return format;
    }

    Format DefaultFormats::getDSVFormat(const Format& format)
    {
        if (format == DefaultFormats::R32G8X24_TYPELESS ||
            format == DefaultFormats::D32_FLOAT_S8X24_UINT ||
            format == DefaultFormats::R32_FLOAT_X8X24_TYPELESS ||
            format == DefaultFormats::X32_TYPELESS_G8X24_UINT)
        {
            return DefaultFormats::D32_FLOAT_S8X24_UINT;
        }
        else if (format == DefaultFormats::R32_TYPELESS ||
            format == DefaultFormats::D32_FLOAT ||
            format == DefaultFormats::R32_FLOAT)
        {
            return DefaultFormats::D32_FLOAT;
        }
        else if (format == DefaultFormats::R24G8_TYPELESS ||
            format == DefaultFormats::D24_UNORM_S8_UINT ||
            format == DefaultFormats::R24_UNORM_X8_TYPELESS ||
            format == DefaultFormats::X24_TYPELESS_G8_UINT)
        {
            return DefaultFormats::D24_UNORM_S8_UINT;
        }
        else if (format == DefaultFormats::R16_TYPELESS ||
            format == DefaultFormats::D16_UNORM ||
            format == DefaultFormats::R16_UNORM)
        {
            return DefaultFormats::D16_UNORM;
        }
        else
        {
            return format;
        }
    }

    Format DefaultFormats::getDepthFormat(const Format& format)
    {
        if (format == DefaultFormats::R32G8X24_TYPELESS ||
            format == DefaultFormats::D32_FLOAT_S8X24_UINT ||
            format == DefaultFormats::R32_FLOAT_X8X24_TYPELESS ||
            format == DefaultFormats::X32_TYPELESS_G8X24_UINT)
        {
            return DefaultFormats::R32_FLOAT_X8X24_TYPELESS;
        }
        else if (format == DefaultFormats::R32_TYPELESS ||
            format == DefaultFormats::D32_FLOAT ||
            format == DefaultFormats::R32_FLOAT)
        {
            return DefaultFormats::R32_FLOAT;
        }
        else if (format == DefaultFormats::R24G8_TYPELESS ||
            format == DefaultFormats::D24_UNORM_S8_UINT ||
            format == DefaultFormats::R24_UNORM_X8_TYPELESS ||
            format == DefaultFormats::X24_TYPELESS_G8_UINT)
        {
            return DefaultFormats::R24_UNORM_X8_TYPELESS;
        }
        else if (format == DefaultFormats::R16_TYPELESS ||
            format == DefaultFormats::D16_UNORM ||
            format == DefaultFormats::R16_UNORM)
        {
            return DefaultFormats::R16_UNORM;
        }
        else
        {
            return DefaultFormats::UNKNOWN;
        }
    }

    Format DefaultFormats::getStencilFormat(const Format& format)
    {
        if (format == DefaultFormats::R32G8X24_TYPELESS ||
            format == DefaultFormats::D32_FLOAT_S8X24_UINT ||
            format == DefaultFormats::R32_FLOAT_X8X24_TYPELESS ||
            format == DefaultFormats::X32_TYPELESS_G8X24_UINT)
        {
            return DefaultFormats::X32_TYPELESS_G8X24_UINT;
        }
        else if (format == DefaultFormats::R24G8_TYPELESS ||
            format == DefaultFormats::D24_UNORM_S8_UINT ||
            format == DefaultFormats::R24_UNORM_X8_TYPELESS ||
            format == DefaultFormats::X24_TYPELESS_G8_UINT)
        {
            return DefaultFormats::X24_TYPELESS_G8_UINT;
        }
        else
        {
            return DefaultFormats::UNKNOWN;
        }
    }

    size_t DefaultFormats::bytesPerPixel(const Format& format)
    {
        switch (format.kind)
        {
        default:
            return 0;

        case Format::Kind::Uncompressed:
            return (format.uncompressed.bits[0] +
                format.uncompressed.bits[1] +
                format.uncompressed.bits[2] +
                format.uncompressed.bits[3]) / 8; // Convert to bytes

        case Format::Kind::Compressed:
            return format.compressed.blockSizeBytes;
        }
    }
}
