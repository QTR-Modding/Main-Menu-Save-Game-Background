#include "SaveGame.h"

std::string SaveGame::GetName(const char* fileName) {
    auto util = RE::BSWin32SaveDataSystemUtility::GetSingleton();
    char fullPath[242];
    util->PrepareFileSavePath(fileName, fullPath, 0, 0);
    return fileName;
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

std::string SaveGame::getPath(const char* a_fileName) {
    auto util = RE::BSWin32SaveDataSystemUtility::GetSingleton();
    char fullPath[242];
    util->PrepareFileSavePath(a_fileName, fullPath, 0, 0);
    std::string path = SaveGame::TrimESS(fullPath) + ".dds";
    return path;
}

std::wstring SaveGame::getWPath(const char* a_fileName) {
    auto path = getPath(a_fileName);
    std::wstring wpath(path.begin(), path.end());
    return wpath;
}