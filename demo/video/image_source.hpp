#pragma once

#include <memory>
#include <mutex>

#include "evg/source.hpp"

namespace lvtk {
namespace demo {

class ImageSource : public evg::Source {
public:
    ImageSource();
    ~ImageSource();

    bool load_file (const std::string& file);
    void process_frame();
    void expose (evg::Context& ctx) override;
    std::mutex& render_mutex() { return _render_mutex; }

private:
    class Impl;
    std::unique_ptr<Impl> impl;
    std::mutex _render_mutex;
    bool data_changed = false;
    int width = 0, height = 0;
};

} // namespace demo
} // namespace lvtk
