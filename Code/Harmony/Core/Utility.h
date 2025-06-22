#pragma once

#include "common.h"

/*
* string utilities.
*/
namespace utility
{
    void print(const char* message);
    std::unique_ptr<char[]> translateErrorCode(const long errorCode) noexcept;

    inline void printF(const char* format, ...)
    {
        constexpr size_t bufferSize = 1024;
        static thread_local char buffer[bufferSize];

        va_list args;
        va_start(args, format);
        vsnprintf(buffer, bufferSize, format, args);
        va_end(args);

        print(buffer);
    }

    std::wstring toWideString(const std::string& str);
    std::string toUTF8(const std::wstring& wstr);

    std::string toLower(const std::string& str);

    std::string getBasePath(const std::string& filePath);
    std::string removeBasePath(const std::string& filePath);

    std::string getFileExtension(const std::string& filePath);
    std::string removeFileExtension(const std::string& filePath);

    std::string timenow() noexcept;
}

/*
* Byte alignment utilities
*/
namespace maths
{
    template <typename T> inline T alignUpWithMask(T value, size_t mask)
    {
        return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T> inline T alignDownWithMask(T value, size_t mask)
    {
        return (T)((size_t)value & ~mask);
    }

    template <typename T> inline T alignUp(T value, size_t alignment)
    {
        return alignUpWithMask(value, alignment - 1);
    }

    template <typename T> inline T alignDown(T value, size_t alignment)
    {
        return alignDownWithMask(value, alignment - 1);
    }

    template <typename T> inline bool isAligned(T value, size_t alignment)
    {
        return 0 == ((size_t)value & (alignment - 1));
    }
}

namespace memory
{
    constexpr size_t KB(long double kilobytes)
    {
        return (size_t)(kilobytes * 1024.0L);
    }

    constexpr size_t MB(long double megabytes)
    {
        return (size_t)(megabytes * 1024.0L * 1024.0L);
    }

    constexpr size_t GB(long double gigabytes)
    {
        return (size_t)(gigabytes * 1024.0L * 1024.0L * 1024.0L);
    }

    constexpr size_t TB(long double terabytes)
    {
        return (size_t)(terabytes * 1024.0L * 1024.0L * 1024.0L * 1024.0L);
    }

//inline constexpr size_t operator"" _KB(size_t kilobytes) { return (size_t)(kilobytes * 1024.0L); }
//inline constexpr size_t operator"" _MB(size_t megabytes) { return (size_t)(megabytes * 1024.0L * 1024.0L); }
//inline constexpr size_t operator"" _GB(size_t gigabytes) { return (size_t)(gigabytes * 1024.0L * 1024.0L * 1024.0L); }
//inline constexpr size_t operator"" _TB(size_t terabytes) { return (size_t)(terabytes * 1024.0L * 1024.0L * 1024.0L * 1024.0L); }
//inline constexpr size_t operator"" _KB(long double kilobytes) { return (size_t)(kilobytes * 1024.0L); }
//inline constexpr size_t operator"" _MB(long double megabytes) { return (size_t)(megabytes * 1024.0L * 1024.0L); }
//inline constexpr size_t operator"" _GB(long double gigabytes) { return (size_t)(gigabytes * 1024.0L * 1024.0L * 1024.0L); }
//inline constexpr size_t operator"" _TB(long double terabytes) { return (size_t)(terabytes * 1024.0L * 1024.0L * 1024.0L * 1024.0L); }
}

/*
* Enum cast to its underlying type.
* Mostly used for using enums as array index.
*/
template <typename Enum>
constexpr auto enum_cast(Enum e) noexcept -> std::underlying_type_t<Enum>
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}


/*
* Deletes the copy constructor and = operator.
* Used to make the code clean.
*/
class NonCopyable
{
protected:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    //NonCopyable(NonCopyable&&) = default;
    //NonCopyable& operator=(NonCopyable&&) = default;
};
