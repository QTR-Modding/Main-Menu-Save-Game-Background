#include "SaveGame.h"
#include <filesystem>
namespace fs = std::filesystem;
std::string SaveGame::GetFullPath(const char* fileName) {
    auto util = RE::BSWin32SaveDataSystemUtility::GetSingleton();
    char fullPath[242];
    util->PrepareFileSavePath(fileName, fullPath, 0, 0);
    return fullPath;
}

std::string SaveGame::TrimESS(const char* name) { 
    std::string str(name);

    //std::transform(str.begin(), str.end(), str.begin(),
    //    [](unsigned char c) { return std::tolower(c); });

    if (str.size() >= 4) {
        auto end = str.end();
        auto it = end - 4;

        if (it[0] == '.' &&
            it[1] == 'e' &&
            it[2] == 's' &&
            it[3] == 's') {
            str.erase(it, end);
        }
    }

    return str;
}

std::string SaveGame::getDDSFullPath(const char* a_fileName) {
    auto fullPath = GetFullPath(a_fileName);
    std::string path = SaveGame::TrimESS(fullPath.c_str()) + ".dds";
    return path;
}

std::string SaveGame::replaceESSWithDDS(const char* a_fileName) {
    std::string path = SaveGame::TrimESS(a_fileName) + ".dds";
    return path;
}

std::wstring SaveGame::getDDSFullWPath(const char* a_fileName) {
    auto path = getDDSFullPath(a_fileName);
    std::wstring wpath(path.begin(), path.end());
    return wpath;
}

std::string SaveGame::GetSavesFolderPath() {
    const char* savesFolder = (const char*)REL::RelocationID(524596, 411236).address();
    const char** savesSlashCharacter = (const char**)REL::RelocationID(500946, 358976).address();
    return std::format("{}{}", savesFolder, savesSlashCharacter[0]);
}

std::vector<std::string> SaveGame::GetAllSaveImages() {
    std::vector<std::pair<std::string, fs::file_time_type>> files;

    std::vector<std::string> result;
    try {

        fs::path folder = GetSavesFolderPath();

        for (auto& entry : fs::directory_iterator(folder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".dds") {
                files.emplace_back(entry.path().string(), fs::last_write_time(entry));
            }
        }

      std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

        for (auto& file : files) {
            result.push_back(file.first);
        }
    } 
    catch (const std::filesystem::filesystem_error& e) {
        logger::error("File not found {}", e.what());
    }

    return result;
}


void SaveGame::DeleteSaveFile(std::filesystem::path file) {
    try {
        if (std::filesystem::exists(file) && std::filesystem::is_regular_file(file)) {
            if (std::filesystem::remove(file)) {
            } else {
                logger::error("Failed to delete file {}", file.string());
            }
        } else {
            logger::error("File not found {}", file.string());
        }
    } catch (const std::filesystem::filesystem_error& e) {
        logger::error("File not found {}", e.what());
    }
}