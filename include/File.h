#pragma once

namespace File {

    bool WriteString(const std::string& file, const std::string& text);
    bool ReadString(const std::string& file, std::string& text);

}