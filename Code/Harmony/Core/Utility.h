#pragma once

#include "common.h"
#include "File.h"

namespace utility
{
    template<typename... Args>
    static inline std::string stringFormat(Args&&... args)
    {
        std::ostringstream oss;
        (oss << ... << args);
        return oss.str();
    }

    static inline std::string timeNow()
    {
        using namespace std::chrono;

        const auto now = system_clock::now();
        const auto now_time_t = system_clock::to_time_t(now);

        std::tm tm{};
        localtime_s(&tm, &now_time_t);

        char buffer[16];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &tm);
        return buffer;
    }

    void print(const std::string& message);
    std::string translateErrorCode(const long errorCode) noexcept;

    std::wstring toWideString(const std::string& str);
    std::string toUTF8(const std::wstring& wstr);
}

namespace maths
{
    template <typename T> inline T divideByMultiple(T value, size_t alignment)
    {
        return static_cast<T>((value + alignment - 1) / alignment);
    }
}

namespace memory
{
    template <typename T> inline T alignUpWithMask(T value, size_t mask)
    {
        return static_cast<T>((static_cast<size_t>(value) + mask) & ~mask);
    }

    template <typename T> inline T alignDownWithMask(T value, size_t mask)
    {
        return static_cast<T>(static_cast<size_t>(value) & ~mask);
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
        return 0 == (static_cast<size_t>(value) & (alignment - 1));
    }

    constexpr size_t KB(long double kilobytes)
    {
        return static_cast<size_t>(kilobytes * 1024.0L);
    }

    constexpr size_t MB(long double megabytes)
    {
        return static_cast<size_t>(megabytes * 1024.0L * 1024.0L);
    }

    constexpr size_t GB(long double gigabytes)
    {
        return static_cast<size_t>(gigabytes * 1024.0L * 1024.0L * 1024.0L);
    }

    constexpr size_t TB(long double terabytes)
    {
        return static_cast<size_t>(terabytes * 1024.0L * 1024.0L * 1024.0L * 1024.0L);
    }

    //inline constexpr size_t operator"" _KB(size_t kilobytes) { return static_cast<size_t>(kilobytes * 1024.0L); }
    //inline constexpr size_t operator"" _MB(size_t megabytes) { return static_cast<size_t>(megabytes * 1024.0L * 1024.0L); }
    //inline constexpr size_t operator"" _GB(size_t gigabytes) { return static_cast<size_t>(gigabytes * 1024.0L * 1024.0L * 1024.0L); }
    //inline constexpr size_t operator"" _TB(size_t terabytes) { return static_cast<size_t>(terabytes * 1024.0L * 1024.0L * 1024.0L * 1024.0L); }
    //inline constexpr size_t operator"" _KB(long double kilobytes) { return static_cast<size_t>(kilobytes * 1024.0L); }
    //inline constexpr size_t operator"" _MB(long double megabytes) { return static_cast<size_t>(megabytes * 1024.0L * 1024.0L); }
    //inline constexpr size_t operator"" _GB(long double gigabytes) { return static_cast<size_t>(gigabytes * 1024.0L * 1024.0L * 1024.0L); }
    //inline constexpr size_t operator"" _TB(long double terabytes) { return static_cast<size_t>(terabytes * 1024.0L * 1024.0L * 1024.0L * 1024.0L); }
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
