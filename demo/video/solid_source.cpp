
#include "solid_source.hpp"
#include "evg/context.hpp"

namespace lvtk {
namespace demo {

static const char* vert_source = R"(
#version 330 core
layout (location = 0) in vec3 pos;
void main() {
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
})";

static const char* frag_source = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4 (1.0f, 0.5f, 0.2f, 1.0f);
}
)";

class SolidSource::Impl {
public:
    Impl (SolidSource& s) : source (s) {}
    ~Impl() {}

    void load_square_buffers (evg::Context& g) {
        if (verts != nullptr)
            return;
        verts.reset (g.get_device().create_vertex_buffer (
            sizeof (evgVec3) * 4, EVG_DYNAMIC));
        index.reset (g.get_device().create_index_buffer (
            sizeof (uint32_t) * 6, EVG_DYNAMIC));
    }

    void load_square_program (evg::Context& g) {
        if (program_linked)
            return;

        if (program == nullptr) {
            vshader.reset (g.reserve_vertex_shader());
            vshader->add_attribute ("pos", EVG_VALUE_VEC3);
            fshader.reset (g.reserve_fragment_shader());
            program.reset (g.reserve_program());
        }

        if (vshader->parse (vert_source))
            if (fshader->parse (frag_source))
                program_linked = program->link (vshader.get(), fshader.get());
    }

    void expose_square (evg::Context& g) {
        if (! verts || ! index)
            return;

        auto pts = (evgVec3*) verts->data();
        float scale = 2.f;
        evg_vec3_set (pts, scale * 0.5f, scale * 0.5f, 0.0f);
        evg_vec3_set (pts + 1, scale * 0.5f, scale * -0.5f, 0.0f);
        evg_vec3_set (pts + 2, scale * -0.5f, scale * -0.5f, 0.0f);
        evg_vec3_set (pts + 3, scale * -0.5f, scale * 0.5f, 0.0f);
        verts->flush();

        auto idx = (uint32_t*) index->data();
        idx[0] = 0;
        idx[1] = 1;
        idx[2] = 3;
        idx[3] = 1;
        idx[4] = 2;
        idx[5] = 3;
        index->flush();

        auto& device = g.get_device();
        device.load_texture (nullptr, 0);
        device.load_index_buffer (index.get());
        device.load_vertex_buffer (verts.get(), 0);
        device.load_vertex_buffer (uv.get(), 1);
        device.load_program (program.get());
        device.draw (EVG_DRAW_MODE_TRIANGLES, 0, 6);
    }

private:
    friend class SolidSource;
    SolidSource& source;
    std::unique_ptr<evg::Program> program;
    std::unique_ptr<evg::Shader> vshader, fshader;
    std::unique_ptr<evg::Buffer> verts;
    std::unique_ptr<evg::Buffer> uv;
    std::unique_ptr<evg::Buffer> index;
    evgMatrix4 projection, sm;
    int proj_key = -1;
    bool verts_changed = true;
    bool program_linked = false;
};

SolidSource::SolidSource() {
    impl.reset (new Impl (*this));
}

SolidSource::~SolidSource() {
    impl.reset();
}

void SolidSource::process_frame() {}

void SolidSource::expose (evg::Context& gc) {
    gc.get_device().enable (EVG_FRAMEBUFFER_SRGB, true);
    impl->load_square_buffers (gc);
    impl->load_square_program (gc);
    impl->expose_square (gc);
    gc.get_device().enable (EVG_FRAMEBUFFER_SRGB, false);
}

} // namespace demo
} // namespace lvtk
