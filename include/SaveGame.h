#pragma once

namespace SaveGame {
    std::string GetName(const char* name);
    std::string TrimESS(const char* name);
    std::string getPath(const char* a_fileName);
    std::wstring getWPath(const char* a_fileName);
}