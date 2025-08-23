#pragma once

#include "common.h"

namespace utility
{
    class File
    {
    public:
        File(const std::string& path) : mPath(path) {}

        inline std::string getPath() const { return mPath; }

        inline bool isModified()
        {
            std::filesystem::file_time_type newModifyTime = std::filesystem::last_write_time(mPath);
            if (newModifyTime > mModifyTime)
            {
                mModifyTime = newModifyTime;
                return true;
            }
            return false;
        }

        ~File() = default;

    public:
        static std::string getBasePath(const std::string& filePath);
        static std::string removeBasePath(const std::string& filePath);
        static std::string getFileExtension(const std::string& filePath);
        static std::string removeFileExtension(const std::string& filePath);

    private:
        std::string mPath;
        std::string mContent;
        std::filesystem::file_time_type mModifyTime;
    };
}
