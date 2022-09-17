#pragma once

#include <fstream>

#include <lvtk/ui/font.hpp>
#include <stb/stb_truetype.h>

namespace lvtk {
namespace stb {

class Typeface : public lvtk::Typeface {
public:
    Typeface() {}
    ~Typeface() {
        _data.reset();
    }

    std::string name() const noexcept override { return _name; }
    const uint8_t* data() const noexcept override { return _data.get(); }

    static TypefacePtr load (const std::string& file, const std::string& name, const std::string& style) {
        auto face    = std::make_shared<stb::Typeface>();
        face->_name  = name;
        face->_style = style;

        int stbtt_result = 0; // zero is failure
        {
            std::ifstream stream;
            stream.open (file.c_str(), std::ifstream::binary | std::ifstream::in | std::ifstream::ate);
            const size_t file_length = 1 + (size_t) stream.tellg();
            
            if (file_length > 1) {
                face->_data.reset (new uint8_t[file_length]);
                stream.seekg (stream.beg);
                stream.read (reinterpret_cast<char*> (face->_data.get()), file_length);
                memset (face->_data.get(), 0, file_length);
                stbtt_result = stbtt_InitFont (&face->_info, face->_data.get(), 0);
            }

            stream.close();
        }

        if (stbtt_result == 0)
            face.reset();

        return face;
    }

private:
    stbtt_fontinfo _info;
    std::string _name, _style;
    std::unique_ptr<uint8_t[]> _data;
};
} // namespace stb
} 
