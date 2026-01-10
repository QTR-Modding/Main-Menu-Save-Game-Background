#pragma once

namespace SaveGame {
    std::string GetFullPath(const char* name);
    std::string TrimESS(const char* name);
    std::string getDDSFullPath(const char* a_fileName);
    std::string replaceESSWithDDS(const char* a_fileName);
    std::wstring getDDSFullWPath(const char* a_fileName);
}