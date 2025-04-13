#pragma once

#include "common.h"

#include <type_traits>

namespace utility
{
    // The platform specific implementation are in their respective project
    void print(const char* message);
    std::unique_ptr<char[]> translateErrorCode(const long errorCode) noexcept;

    template<typename... Args>
    inline std::string formatMessage(const char* format, Args... args)
    {
        int size = std::snprintf(nullptr, 0, format, args...);
        if (size <= 0) return "";

        std::string result(size + 1, '\0'); // +1 for null terminator
        std::snprintf(&result[0], result.size(), format, args...);

        result.pop_back(); // remove null terminator
        return result;
    }

    template<typename ... Args>
    inline void printF(const char* format, Args ... args)
    {
        std::string formattedMessage = formatMessage(format, args...);
        print(formattedMessage.c_str());
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
