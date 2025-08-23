#include "File.h"

namespace utility
{

    std::string File::getBasePath(const std::string& filePath)
    {
        size_t lastSlash;
        if ((lastSlash = filePath.rfind('/')) != std::string::npos)
            return filePath.substr(0, lastSlash + 1);
        else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
            return filePath.substr(0, lastSlash + 1);
        else
            return "";
    }

    std::string File::removeBasePath(const std::string& filePath)
    {
        size_t lastSlash;
        if ((lastSlash = filePath.rfind('/')) != std::string::npos)
            return filePath.substr(lastSlash + 1, std::string::npos);
        else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
            return filePath.substr(lastSlash + 1, std::string::npos);
        else
            return filePath;
    }

    std::string File::getFileExtension(const std::string& filePath)
    {
        std::string fileName = removeBasePath(filePath);
        size_t extOffset = fileName.rfind('.');
        if (extOffset == std::wstring::npos)
            return "";

        return fileName.substr(extOffset + 1);
    }

    std::string File::removeFileExtension(const std::string& filePath)
    {
        return filePath.substr(0, filePath.rfind("."));
    }
}
