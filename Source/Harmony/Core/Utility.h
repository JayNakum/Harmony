#pragma once

#include "common.h"

#include <type_traits>
#include <stdarg.h>

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

    inline std::string timenow() noexcept
    {
        time_t m_time = time(NULL);
        struct tm now = {};

        localtime_s(&now, &m_time);

        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &now);

        return std::string(buffer);
    }
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
class NonCopyableButMovable
{
protected:
    NonCopyableButMovable() = default;
    NonCopyableButMovable(const NonCopyableButMovable&) = delete;
    NonCopyableButMovable& operator=(const NonCopyableButMovable&) = delete;

    NonCopyableButMovable(NonCopyableButMovable&&) = default;
    NonCopyableButMovable& operator=(NonCopyableButMovable&&) = default;
};
