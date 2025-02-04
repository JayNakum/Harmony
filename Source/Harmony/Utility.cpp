#include "Utility.h"

namespace utility
{
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

    std::string toLower(const std::string& str)
    {
        std::string lower_case = str;
        std::locale loc;
        for (char& s : lower_case)
            s = std::tolower(s, loc);
        return lower_case;
    }

    std::string getBasePath(const std::string& filePath)
    {
        size_t lastSlash;
        if ((lastSlash = filePath.rfind('/')) != std::string::npos)
            return filePath.substr(0, lastSlash + 1);
        else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
            return filePath.substr(0, lastSlash + 1);
        else
            return "";
    }

    std::string removeBasePath(const std::string& filePath)
    {
        size_t lastSlash;
        if ((lastSlash = filePath.rfind('/')) != std::string::npos)
            return filePath.substr(lastSlash + 1, std::string::npos);
        else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
            return filePath.substr(lastSlash + 1, std::string::npos);
        else
            return filePath;
    }

    std::string getFileExtension(const std::string& filePath)
    {
        std::string fileName = removeBasePath(filePath);
        size_t extOffset = fileName.rfind('.');
        if (extOffset == std::wstring::npos)
            return "";

        return fileName.substr(extOffset + 1);
    }

    std::string removeFileExtension(const std::string& filePath)
    {
        return filePath.substr(0, filePath.rfind("."));
    }

}
