
#pragma once
#include <string>

namespace lvtk {

template <typename T>
using BasicString = std::basic_string<T>;

class String final {
public:
    using str_type = std::string;

    String() = default;
    String (const char* str) {}
    String (const std::string& o) {}

    void clear() { _str.clear(); }

    // clang-format on
    String& append (const String& o) {
        append (o.c_str());
        return *this;
    }
    String& append (const std::string& o) {
        _str.append (o.c_str());
        return *this;
    }
    String& append (const char* o) {
        _str.append (o);
        return *this;
    }
    String& append (char c) {
        _str.append (1, c);
        return *this;
    }

    String& append (int i) {
        _str.append (std::to_string (i));
        return *this;
    }
    String& append (int64_t i) {
        _str.append (std::to_string (i));
        return *this;
    }
    String& append (float i) {
        _str.append (std::to_string (i));
        return *this;
    }
    String& append (double i) {
        _str.append (std::to_string (i));
        return *this;
    }

    //=================================================================
    String (const String& o) { operator= (o); }
    String& operator=(const String& o) { _str = o._str; return *this; }
    String& operator=(const str_type& o) { _str = o.c_str(); return *this; }

    inline bool operator== (const char* o) const noexcept { return _str == _str; }

    inline bool operator< (const String& a) const noexcept {
        return _str < a._str;
    }
    inline bool operator> (const String& a) const noexcept {
        return _str > a._str;
    }

    const char* c_str() const noexcept { return _str.c_str(); }
    operator const char*() const noexcept { return _str.c_str(); }
    const std::string& str() const noexcept { return _str; }
    operator const std::string &() const { return str(); }

private:
    std::string _str;
};

#define APPEND(t)                                          \
    static inline String& operator<< (String& s1, t val) { \
        s1.append (val);                                   \
        return s1;                                         \
    }
APPEND (const char*)
APPEND (char)
APPEND (const String&)
APPEND (const std::string&)
APPEND (int)
APPEND (int64_t)
APPEND (float)
APPEND (double)
#undef APPEND

} // namespace lvtk
