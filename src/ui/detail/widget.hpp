
#pragma once

namespace lvtk {
namespace detail {

class Widget {
public:
    Widget (lvtk::Widget& o) : owner (o) {

    }

private:
    friend class lvtk::Widget;
    friend class lvtk::Main;
    friend class detail::Main;
    friend class lvtk::View;
    friend class detail::View;

    lvtk::Widget& owner;
    lvtk::Widget* parent = nullptr;
    std::unique_ptr<lvtk::View> view;
    std::vector<lvtk::Widget*> widgets;
    Rectangle<int> bounds;
    bool visible { false };
    bool opaque { false };
};

}
}
