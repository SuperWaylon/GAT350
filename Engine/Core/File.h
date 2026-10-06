#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace nu
{
    std::string GetWorkingDirectory();
    bool SetWorkingDirectory(const std::string& path);

    std::string GetFilename(const std::string& path);
    std::string GetFileExtension(const std::string& path);
    std::string GetFilenameNoExtension(const std::string& path);
    
    bool FileExists(const std::string& path);

    std::vector<std::string> GetFilesInDirectory(const std::string& path);
    std::vector<std::string> GetDirectoriesIn(const std::string& path);

    bool ReadTextFile(const std::string& path, std::string& data);
    bool WriteTextFile(const std::string& path, const std::string& data, bool append = false);
}