#include "SaveGame.h"

std::string SaveGame::GetName(const char* fileName) {
    auto util = RE::BSWin32SaveDataSystemUtility::GetSingleton();
    char fullPath[242];
    util->PrepareFileSavePath(fileName, fullPath, 0, 0);
    return fileName;
}

std::string SaveGame::LowerTrimESS(const char* name) { 
    std::string str(name);

    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::tolower(c); });

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
