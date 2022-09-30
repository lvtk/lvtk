// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <vector>

namespace lvtk {

/** A type of operation used when processing path data
    @ingroup graphics
    @headerfile lvtk/ui/path.hpp
*/
enum class PathOp {
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

    /** Clear the path */
    void clear() { _data.clear(); }

    /** Move to x y */
    void move (float x, float y) {
        add_op (PathOp::MOVE, x, y);
    }

    /** Line to x y */
    void line (float x, float y) {
        if (_data.empty())
            move (0, 0);
        add_op (PathOp::LINE, x, y);
    }

    /** Line to from a Point */
    template <class Pt>
    void line (Pt pt) {
        line (static_cast<float> (pt.x), static_cast<float> (pt.y));
    }

    /** Quadratic to */
    void quad (float x1, float y1, float x2, float y2) {
        if (_data.empty())
            move (0, 0);
        add_op (PathOp::QUADRATIC, x1, y1, x2, y2);
    }

    /** Cubic to */
    void cubic (float x1, float y1,
                float x2, float y2,
                float x3, float y3) {
        if (_data.empty())
            move (0, 0);
        add_op (PathOp::CUBIC, x1, y1, x2, y2, x3, y3);
    }

    /** Close the path */
    void close() {
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

} // namespace lvtk
