// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <algorithm>

#include <lvtk/string.hpp>

namespace lvtk {

/** A point. x y coordinate.
    @ingroup widgets
*/
template <typename Val>
class Point {
public:
    Val x {}, y {};

    template <typename T>
    Point<T> as() const noexcept {
        return {
            static_cast<T> (x),
            static_cast<T> (y)
        };
    }

    Point<Val> operator- (Point o) const noexcept {
        Point copy (*this);
        copy -= o;
        return copy;
    }

    Point<Val>& operator-= (Point o) noexcept {
        x -= o.x;
        y -= o.y;
        return *this;
    }

    Point<Val> operator+ (Point<Val> o) const noexcept {
        Point copy (*this);
        copy += o;
        return copy;
    }

    Point<Val>& operator+= (Point<Val> o) noexcept {
        x += o.x;
        y += o.y;
        return *this;
    }

    template <typename OT>
    Point operator* (OT m) const noexcept {
        using T = typename std::common_type<Val, OT>::type;
        return { (T) ((T) x * (T) m),
                 (T) ((T) y * (T) m) };
    }

    template <typename OT>
    Point operator/ (OT d) const noexcept {
        using T = typename std::common_type<Val, OT>::type;
        return { (T) ((T) x / (T) d),
                 (T) ((T) y / (T) d) };
    }

    std::string str() const noexcept {
        String st;
        st << x << " " << y;
        return st;
    }
};

} // namespace lvtk
