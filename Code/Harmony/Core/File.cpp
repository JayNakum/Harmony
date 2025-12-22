#include "File.h"

namespace utility
{
    File::File(const std::string& path)
        : mPath(path)
    {
        mStream.clear();
        mStream.open(
            mPath,
            std::ios::in | std::ios::out | std::ios::app
        );
    }

    bool File::isModified()
    {
        std::filesystem::file_time_type newModifyTime = std::filesystem::last_write_time(mPath);
        if (newModifyTime > mModifyTime)
        {
            mModifyTime = newModifyTime;
            return true;
        }
        return false;
    }

    void File::write(const std::string& text)
    {
        if (!mStream.is_open())
            return;

        mStream << text;
        mStream.flush();
    }

    std::string File::read()
    {
        if (!mStream.is_open())
            return {};

        const auto pos = mStream.tellg();

        mStream.seekg(0, std::ios::beg);

        std::ostringstream oss;
        oss << mStream.rdbuf();

        mStream.clear();
        mStream.seekg(pos);

        return oss.str();
    }

    File::~File()
    {
        if (mStream.is_open())
        {
            mStream.close();
        }
    }
}

namespace utility
{
    std::string File::getBasePath(const std::string& filePath)
    {
        size_t lastSlash;
        if ((lastSlash = filePath.rfind('/')) != std::string::npos)
        {
            return filePath.substr(0, lastSlash + 1);
        }
        else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
        {
            return filePath.substr(0, lastSlash + 1);
        }
        else
        {
            return "";
        }
    }

    std::string File::removeBasePath(const std::string& filePath)
    {
        size_t lastSlash;
        if ((lastSlash = filePath.rfind('/')) != std::string::npos)
        {
            return filePath.substr(lastSlash + 1, std::string::npos);
        }
        else if ((lastSlash = filePath.rfind('\\')) != std::string::npos)
        {
            return filePath.substr(lastSlash + 1, std::string::npos);
        }
        else
        {
            return filePath;
        }
    }

    std::string File::getFileExtension(const std::string& filePath)
    {
        std::string fileName = removeBasePath(filePath);
        size_t extOffset = fileName.rfind('.');
        if (extOffset == std::wstring::npos)
        {
            return "";
        }

        return fileName.substr(extOffset + 1);
    }

    std::string File::removeFileExtension(const std::string& filePath)
    {
        return filePath.substr(0, filePath.rfind("."));
    }
}

namespace utility
{
    FileMT::FileMT(const std::string& path)
        : File(path) {}

    void FileMT::writeLine(const std::string& line)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        write(line);
        write("\n");
    }

    void FileMT::write(const std::string& line)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        File::write(line);
    }

    std::string FileMT::read()
    {
        std::lock_guard<std::mutex> lock(mMutex);
        return File::read();
    }
}
