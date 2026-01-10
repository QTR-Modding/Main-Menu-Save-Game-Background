#include "SaveGame.h"

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