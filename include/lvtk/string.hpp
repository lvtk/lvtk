
#pragma once
#include <string>

template <typename T>
using BasicString = std::basic_string<T>;

namespace lvtk {
class String : public BasicString<char> {
public:
    String() = default;
    String (const char* str) : Base (str) {}
    String (const std::string& o) : Base (o) {}

private:
    using Base = BasicString<char>;
};
} // namespace lvtk
