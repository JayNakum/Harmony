#pragma once

#include "pch.h"

namespace utility
{
    class File
    {
    public:
        File(const std::string& path);

        inline const std::filesystem::path& getPath() const { return mPath; }

        bool isModified();

        void write(const std::string& text);

        std::string read();

        File(const File&) = delete;
        File& operator=(const File&) = delete;

        File(File&&) noexcept = default;
        File& operator=(File&&) noexcept = default;

        virtual ~File();

    public:
        static std::string getBasePath(const std::string& filePath);
        static std::string removeBasePath(const std::string& filePath);
        static std::string getFileExtension(const std::string& filePath);
        static std::string removeFileExtension(const std::string& filePath);

    private:
        std::filesystem::path mPath;
        std::fstream mStream;
        std::filesystem::file_time_type mModifyTime;
    };

    class FileMT : public File
    {
    public:
        FileMT(const std::string& path);

        void writeLine(const std::string& line);

        void write(const std::string& text);

        std::string read();

    private:
        std::mutex mMutex;
    };
}
