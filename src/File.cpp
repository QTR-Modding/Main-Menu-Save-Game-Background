#include "File.h"

bool File::WriteString(const std::string& file, const std::string& text) {
    std::ofstream out(file, std::ios::binary);
    if (!out) return false;
    out.write(text.data(), text.size());
    return out.good();
}

bool File::ReadString(const std::string& file, std::string& text) {
    std::ifstream in(file, std::ios::binary);
    if (!in) return false;
    in.seekg(0, std::ios::end);
    std::size_t size = static_cast<std::size_t>(in.tellg());
    in.seekg(0, std::ios::beg);
    text.resize(size);
    in.read(text.data(), size);
    return in.good() || in.eof();
}