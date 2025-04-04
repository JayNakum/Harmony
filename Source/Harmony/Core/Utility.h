#pragma once

#include "common.h"

#include <type_traits>

namespace utility
{
    // The platform specific implementation are in their respective project
    void print(const char* message);
    std::unique_ptr<char[]> translateErrorCode(const long errorCode) noexcept;

    template<typename ... Args>
    inline std::unique_ptr<char[]> formatMessage(const char* format, Args ... args)
    {
        int size = snprintf(nullptr, 0, format, args ...) + 1; // +1 for null terminator

        std::unique_ptr<char[]> buffer(new char[size]);

        snprintf(buffer.get(), size, format, args ...);

        return buffer;
    }

    template<typename ... Args>
    inline void printF(const char* format, Args ... args)
    {
        std::unique_ptr<char[]> formattedMessage = formatMessage(format, args...);
        print(formattedMessage.get());
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
};

struct MemoryBlob
{
    size_t size;
    void* ptr;
};
