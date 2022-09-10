
#pragma once

#include <memory>
#include <mutex>

#include "evg/source.hpp"

namespace lvtk {
namespace demo {

class SolidSource : public evg::Source {
public:
    SolidSource();
    ~SolidSource();
    void process_frame();
    std::mutex& render_mutex() { return _render_mutex; }
    void expose (evg::Context& gc) override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
    std::mutex _render_mutex;
};

} // namespace demo
} // namespace lvtk
