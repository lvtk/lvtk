// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <cstdint>
#include <cstring>
#include <string>

namespace lvtk {

/** A typical String type.
    
    Can be passed to functions which take const char* or std::string
    as an argument.

    @headerfile lvtk/string.hpp
    @ingroup lvtk
 */
class String final {
public:
    using str_type = std::string;

    /** Create an empty string */
    String() = default;
    /** Create a string from a const char */
    String (const char* str) : _str (str) {}
    /** Create a string from a std::string */
    String (const std::string& o) : _str (o) {}

    /** Clear this string. */
    void clear() { _str.clear(); }

    // clang-format on

    /** Append another String to this one */
    String& append (const String& o) {
        append (o.c_str());
        return *this;
    }

    /** Append a std::string to this one */
    String& append (const std::string& o) {
        _str.append (o.c_str());
        return *this;
    }

    /** Append a C string to this one */
    String& append (const char* o) {
        _str.append (o);
        return *this;
    }

    /** append a single char. */
    String& append (char c) {
        _str.append (1, c);
        return *this;
    }

    /** Append an int */
    String& append (int i) {
        _str.append (std::to_string (i));
        return *this;
    }

    /** Append an int64 */
    String& append (int64_t i) {
        _str.append (std::to_string (i));
        return *this;
    }

    /** Append a float */
    String& append (float i) {
        _str.append (std::to_string (i));
        return *this;
    }

    /** Append a double */
    String& append (double i) {
        _str.append (std::to_string (i));
        return *this;
    }

    //=================================================================
    String (const String& o) { _str = o._str; }
    String& operator= (const String& o) {
        _str = o._str;
        return *this;
    }
    String& operator= (const str_type& o) {
        _str = o.c_str();
        return *this;
    }
    String& operator= (const char* o) {
        _str = o;
        return *this;
    }

    inline bool operator== (const char* o) const noexcept { return strcmp (_str.c_str(), o) == 0; }
    inline bool operator== (const String& o) const noexcept { return _str == o._str; }
    inline bool operator== (const std::string& o) const noexcept { return _str == o; }

    inline bool operator!= (const char* o) const noexcept { return strcmp (_str.c_str(), o) != 0; }
    inline bool operator!= (const String& o) const noexcept { return _str != o._str; }
    inline bool operator!= (const std::string& o) const noexcept { return _str != o; }

    inline bool operator<(const String& a) const noexcept {
        return _str < a._str;
    }
    inline bool operator> (const String& a) const noexcept {
        return _str > a._str;
    }

    /** Returns the C string of this String */
    const char* c_str() const noexcept { return _str.c_str(); }
    operator const char*() const noexcept { return _str.c_str(); }
    const std::string& str() const noexcept { return _str; }
    operator const std::string&() const { return str(); }

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
