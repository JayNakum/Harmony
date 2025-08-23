#include "common.h"
#include "win.h"

#include "Utility.h"

/*
* Windows specific overrides of the utility functions
*/
namespace utility
{
    void print(const char* message)
    {
    #if defined(DEBUG)
        OutputDebugStringA(message);
    #endif
        (void)message;
    }

    std::string translateErrorCode(const long errorCode) noexcept
    {
        HRESULT hResult = static_cast<HRESULT>(errorCode);
        if (FACILITY_WINDOWS == HRESULT_FACILITY(errorCode))
        {
            hResult = HRESULT_CODE(errorCode);
        }

        LPSTR szErrMsg = nullptr;

        if (FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, (DWORD)hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&szErrMsg, 0, nullptr) != 0)
        {
            std::string errorMessage(szErrMsg);
            LocalFree(szErrMsg);
            return errorMessage;
        }
        else
        {
            char buffer[128];
            snprintf(buffer, sizeof(buffer), "Unexpected error # %#x.", (UINT)hResult);
            return std::string(buffer);
        }
    }

    std::wstring toWideString(const std::string& str)
    {
        wchar_t wstr[MAX_PATH];
        if (!MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1, wstr, MAX_PATH))
            wstr[0] = L'\0';
        return wstr;
    }

    std::string toUTF8(const std::wstring& wstr)
    {
        char str[MAX_PATH];
        if (!WideCharToMultiByte(CP_ACP, MB_PRECOMPOSED, wstr.c_str(), -1, str, MAX_PATH, nullptr, nullptr))
            str[0] = L'\0';
        return str;
    }
}
