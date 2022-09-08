// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <vector>

namespace lvtk {
namespace graphics {

/** A type of operation used when processing path data */
enum class PathOp {
    MOVE = 100000,
    LINE,
    QUADRATIC,
    CUBIC,
    CLOSE
};

/** The element_type for BasicPath<T>::iterator */
template<typename Coord>
struct PathItem {
    PathOp type { PathOp::MOVE };
    Coord x1 {}, y1 {},
          x2 {}, y2 {},
          x3 {}, y3 {};
};

template<typename Coord>
class BasicPath {
public:
    using coord_type = Coord;
    using vector_type = std::vector<Coord>;
    using value_type = PathItem<coord_type>;

    BasicPath() = default;
    ~BasicPath() = default;

    void clear() { _data.clear(); }

    void move (coord_type x, coord_type y) {
        add_op (PathOp::MOVE, x, y);
    }

    void line (coord_type x, coord_type y) {
        if (_data.empty())
            move (0, 0);
        add_op (PathOp::LINE, x, y);
    }

    template <class Pt>
    void line (Pt pt) {
        line (static_cast<coord_type> (pt.x), static_cast<coord_type> (pt.y));
    }

    void quad (coord_type x1, coord_type y1, coord_type x2, coord_type y2) {
        if (_data.empty())
            move (0, 0);
        add_op (PathOp::QUADRATIC, x1, y1, x2, y2);
    }

    void cubic (coord_type x1, coord_type y1,
                coord_type x2, coord_type y2,
                coord_type x3, coord_type y3) {
        if (_data.empty())
            move (0, 0);
        add_op (PathOp::CUBIC, x1, y1, x2, y2, x3, y3);
    }

    void close() {
        if (! _data.empty() && static_cast<coord_type> (PathOp::CLOSE) != _data.back())
            add_op (PathOp::CLOSE);
    }

    struct iterator {
        iterator& operator++() {
            _item.type = static_cast<PathOp> (*_i++);
            switch (_item.type) {
                case PathOp::MOVE:
                    _item.x1 = *_i++;
                    _item.y1 = *_i++;
                    break;
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
                    _item.x1 = _item.y1 =
                        _item.x2 = _item.y2 =
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
        friend class BasicPath;
        explicit iterator (typename vector_type::const_iterator i) : _i (i) {}
        typename vector_type::const_iterator _i;
        value_type _item;
    };

    auto begin() const noexcept { return iterator (_data.cbegin()); }
    auto end() const noexcept { return iterator (_data.cend()); }

    const auto& data() const noexcept { return _data; }
    void reserve (std::size_t n) { _data.reserve (_data.size() + n); }

private:
    vector_type _data;
    template <typename T>
    void add_op (T v) { _data.push_back (static_cast<coord_type> (v)); }
    template <typename T, typename... Args>
    void add_op (T op, Args... args) {
        add_op (static_cast<coord_type> (op));
        add_op (args...);
    }
};

} // namespace graphics
} // namespace lvtk
