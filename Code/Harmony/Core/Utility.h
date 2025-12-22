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
