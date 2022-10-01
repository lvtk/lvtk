// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <vector>

namespace lvtk {

/** A type of operation used when processing path data
    @ingroup graphics
    @headerfile lvtk/ui/path.hpp
*/
enum class PathOp : int {
    MOVE = 100000, ///< Move to
    LINE,          ///< Line to
    QUADRATIC,     ///< Quad to
    CUBIC,         ///< Cubic to
    CLOSE          ///< Close path
};

/** The element_type for Path<T>::iterator
    @ingroup graphics
    @headerfile lvtk/ui/path.hpp
*/
struct PathItem final {
    /** Type of operation */
    PathOp type { PathOp::MOVE };
    /** X1 coord */
    float x1 {};
    /** Y1 coord */
    float y1 {};
    /** X2 coord */
    float x2 {};
    /** Y2 coord */
    float y2 {};
    /** X3 coord */
    float x3 {};
    /** Y3 coord */
    float y3 {};
};

/** Drawable path.
    @ingroup graphics
    @headerfile lvtk/ui/path.hpp
*/
class Path {
public:
    using value_type = PathItem;

    /** Make an empty path */
    Path()  = default;
    ~Path() = default;

    /** Copy a path */
    Path (const Path& o) { operator= (o); }
    Path& operator= (const Path& o) {
        std::copy (o._data.begin(), o._data.end(), _data.begin());
        return *this;
    }

    /** Move a path */
    Path (Path&& o) : _data (std::move (o._data)) {}
    Path& operator= (Path&& o) {
        this->_data = std::move (o._data);
        return *this;
    }

    /** Clear the path */
    void clear() { _data.clear(); }

    void begin_path() {
        clear();
    }

    /** Move to x y */
    void move_to (float x, float y) {
        add_op (PathOp::MOVE, x, y);
    }

    /** Line to x y */
    void line_to (float x, float y) {
        if (_data.empty())
            move_to (0.f, 0.f);
        add_op (PathOp::LINE, x, y);
    }

    /** Line to from a Point */
    template <class Pt>
    void line_to (Pt pt) {
        line_to (static_cast<float> (pt.x), static_cast<float> (pt.y));
    }

    /** Quadratic to */
    void quad_to (float x1, float y1, float x2, float y2) {
        if (_data.empty())
            move_to (0, 0);
        add_op (PathOp::QUADRATIC, x1, y1, x2, y2);
    }

    /** Cubic to */
    void cubic_to (float x1, float y1,
                   float x2, float y2,
                   float x3, float y3) {
        if (_data.empty())
            move_to (0, 0);
        add_op (PathOp::CUBIC, x1, y1, x2, y2, x3, y3);
    }

    /** Close the path */
    void close_path() {
        if (! _data.empty() && static_cast<float> (PathOp::CLOSE) != _data.back())
            add_op (PathOp::CLOSE);
    }

    /** Item iterator */
    struct iterator {
        iterator& operator++() {
            _item.type = static_cast<PathOp> (*_i++);
            switch (_item.type) {
                case PathOp::MOVE:
                case PathOp::LINE:
                    _item.x1 = *_i++;
                    _item.y1 = *_i++;
                    break;
                case PathOp::QUADRATIC:
                    _item.x1 = *_i++;
                    _item.y1 = *_i++;
                    _item.x2 = *_i++;
                    _item.y2 = *_i++;
                    break;
                case PathOp::CUBIC:
                    _item.x1 = *_i++;
                    _item.y1 = *_i++;
                    _item.x2 = *_i++;
                    _item.y2 = *_i++;
                    _item.x3 = *_i++;
                    _item.y3 = *_i++;
                    break;
                case PathOp::CLOSE:
                    _item.x1         = _item.y1 =
                        _item.x2     = _item.y2 =
                            _item.x3 = _item.y3 = 0.f;
                    break;
                default:
                    break;
            }
            return *this;
        }

        iterator operator++ (int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        const value_type& operator*() const {
            return _item;
        }

        bool operator== (const iterator& o) { return _i == o._i; }
        bool operator!= (const iterator& o) { return _i != o._i; };

    private:
        friend class Path;
        explicit iterator (std::vector<float>::const_iterator i) : _i (i) {}
        std::vector<float>::const_iterator _i;
        value_type _item;
    };

    /** Begin iter */
    auto begin() const noexcept { return iterator (_data.cbegin()); }
    /** End iter */
    auto end() const noexcept { return iterator (_data.cend()); }
    /** Raw data vector */
    const auto& data() const noexcept { return _data; }
    /** Reserve amount of bytes */
    void reserve (std::size_t n) { _data.reserve (_data.size() + n); }

private:
    std::vector<float> _data;
    template <typename T>
    void add_op (T v) { _data.push_back (static_cast<float> (v)); }
    template <typename T, typename... Args>
    void add_op (T op, Args... args) {
        add_op (static_cast<float> (op));
        add_op (args...);
    }
};

namespace graphics {

/** Add a rounded rectangle to something.
    
    @tparam Pth object implementing path methods
    @param path object to use
    @param x X coord
    @param y Y coord
    @param w Width
    @param h Height
    @param corner_size_x Corner Size in X direction
    @param corner_size_y Corner Size in y direction
    @param top_left Round top left
    @param top_right Round top right
    @param bottom_left Round bottom left
    @param bottom_right Round bottom right

    @returns Pth for daisy chaining

    @ingroup graphics
    @see Path, DrawingContext, Graphics
*/
template <class Pth>
inline static Pth&& rounded_rect (Pth&& path, const float x, const float y,
                                  const float w, const float h,
                                  float corner_size_x, float corner_size_y,
                                  const bool top_left, const bool top_right,
                                  const bool bottom_left, const bool bottom_right) {
    corner_size_x = std::min (corner_size_x, w * 0.5f);
    corner_size_y = std::min (corner_size_y, h * 0.5f);
    auto cs45x    = corner_size_x * 0.45f;
    auto cs45y    = corner_size_y * 0.45f;
    auto x2       = x + w;
    auto y2       = y + h;

    if (top_left) {
        path.move_to (x, y + corner_size_y);
        path.cubic_to (x, y + cs45y, x + cs45x, y, x + corner_size_x, y);
    } else {
        path.move_to (x, y);
    }

    if (top_right) {
        path.line_to (x2 - corner_size_x, y);
        path.cubic_to (x2 - cs45x, y, x2, y + cs45y, x2, y + corner_size_y);
    } else {
        path.line_to (x2, y);
    }

    if (bottom_right) {
        path.line_to (x2, y2 - corner_size_y);
        path.cubic_to (x2, y2 - cs45y, x2 - cs45x, y2, x2 - corner_size_x, y2);
    } else {
        path.line_to (x2, y2);
    }

    if (bottom_left) {
        path.line_to (x + corner_size_x, y2);
        path.cubic_to (x + cs45x, y2, x, y2 - cs45y, x, y2 - corner_size_y);
    } else {
        path.line_to (x, y2);
    }

    path.close_path();
    return path;
}

/** Rounded rect from x,y,w,h, and corner_size.
    @tparam Pth Type of path object used
    @param po Object implementing path methods
    @param x X coordinate.
    @param y Y coordinate
    @param width Width of rectangle
    @param height Height of rectangle
    @param corner_size Corner size to use

    @returns Pth for daisy chaining

    @ingroup graphics
    @see Path, DrawingContext, Graphics
 */
template <class Pth>
inline static Pth&& rounded_rect (Pth&& po, const float x, const float y,
                                  const float width, const float height, float corner_size) {
    return rounded_rect (
        po, x, y, width, height, corner_size, corner_size, true, true, true, true);
}

} // namespace graphics
} // namespace lvtk
