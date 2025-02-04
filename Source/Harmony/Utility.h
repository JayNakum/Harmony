#pragma once

#include "common.h"
#include "win.h"

namespace utility
{
    inline void print(const char* message)
    {
        OutputDebugStringA(message); // TODO: support file logging
    }

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

    inline std::string timenow()
    {
        time_t m_time = time(NULL);
        struct tm now = {};

        localtime_s(&now, &m_time);

        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &now);

        return std::string(buffer);
    }

    inline std::unique_ptr<char[]> translateHResult(const HRESULT errorCode) noexcept
    {
        HRESULT hr = errorCode;
        if (FACILITY_WINDOWS == HRESULT_FACILITY(errorCode))
        {
            hr = HRESULT_CODE(errorCode);
        }

        LPSTR szErrMsg = nullptr;

        if (FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&szErrMsg, 0, NULL) != 0)
        {
            size_t bufferSize = strlen(szErrMsg) + 1;

            std::unique_ptr<char[]> errorMessage(new char[bufferSize]);
            strcpy_s(errorMessage.get(), bufferSize, szErrMsg);

            LocalFree(szErrMsg);

            return errorMessage;
        }
        else
        {
            const char* genericErrorMsg = "Unexpected error # % #x.";
            size_t bufferSize = snprintf(nullptr, 0, genericErrorMsg, hr) + 1;

            std::unique_ptr<char[]> errorMessage(new char[bufferSize]);
            snprintf(errorMessage.get(), bufferSize, genericErrorMsg, hr);

            return errorMessage;
        }
    }
}
