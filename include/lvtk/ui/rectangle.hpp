// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <lvtk/string.hpp>

namespace lvtk {

template <typename Val>
struct Point {
    Val x {}, y {};

    template <typename T>
    Point<T> as() const noexcept {
        return {
            static_cast<T> (x),
            static_cast<T> (y)
        };
    }

    Point<Val> operator- (Point<Val> o) const noexcept {
        Point copy (*this);
        copy -= o;
        return copy;
    }

    Point<Val>& operator-= (Point<Val> o) noexcept {
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

template <typename Val>
struct Rectangle {
    Val x {}, y {}, width {}, height {};
    Point<Val> pos() const noexcept { return { x, y }; }
    bool empty() const noexcept { return width <= Val() || height <= Val(); }

    /** convert to a differnt value type */
    template <typename T>
    Rectangle<T> as() const noexcept {
        return {
            static_cast<T> (x),
            static_cast<T> (y),
            static_cast<T> (width),
            static_cast<T> (height)
        };
    }

    /** Returns a copy of this shape with top-left at x y */
    Rectangle<Val> at (Val x, Val y) const noexcept {
        return { x, x, width, height };
    }

    Rectangle<Val> at (Val xy) const noexcept {
        return { xy, xy, width, height };
    }

    std::string str() const noexcept {
        String out;
        out << x << " " << y << " " << width << " " << height;
        return out;
    }

    bool contains (Val x1, Val y1) const noexcept {
        return x1 >= x && y1 >= y && x1 < x + width && y1 < y + height;
    }

    bool contains (Point<Val> pt) const noexcept {
        return pt.x >= x && pt.y >= y && pt.x < x + width && pt.y < y + height;
    }

    bool contains (Rectangle other) const noexcept {
        return x <= other.x && y <= other.y
               && x + width >= other.x + other.width
               && y + height >= other.y + other.height;
    }

    bool operator== (const Rectangle<Val>& o) const noexcept {
        return x == o.x && y == o.y && width == o.width && height == o.height;
    }

    Rectangle<Val> operator+ (Point<Val> delta) const noexcept {
        return { x + delta.x, y + delta.y };
    }
    Rectangle<Val>& operator+= (Point<Val> delta) noexcept {
        x += delta.x;
        y += delta.y;
        return *this;
    }
    Rectangle<Val> operator- (Point<Val> delta) const noexcept {
        return { x - delta.x, y - delta.y };
    }
    Rectangle<Val>& operator-= (Point<Val> delta) noexcept {
        x -= delta.x;
        y -= delta.y;
        return *this;
    }

    template <typename F>
    Rectangle<Val>& operator*= (F scale) noexcept {
        x *= scale;
        y *= scale;
        width *= scale;
        height *= scale;
        return *this;
    }

    template <typename F>
    Rectangle<Val> operator* (F scale) const noexcept {
        Rectangle r (*this);
        r *= scale;
        return r;
    }

    template <typename F>
    Rectangle<Val>& operator/= (F scale) noexcept {
        x /= scale;
        y /= scale;
        width /= scale;
        height /= scale;
        return *this;
    }

    template <typename F>
    Rectangle<Val> operator/ (F scale) const noexcept {
        Rectangle r (*this);
        r /= scale;
        return r;
    }

    Rectangle<Val>& reduce (Val dx, Val dy) {
        x -= dx;
        y -= dy;
        width += (dx * 2);
        height += (dy * 2);
        return *this;
    }

    Rectangle<Val>& reduce (Val delta) {
        return reduce (delta, delta);
    }

    /** Returns an expanded shape at same center.
        @param amount How much to grow by.
    */
    Rectangle<Val> bigger (Val amount) const noexcept {
        auto r = *this;
        return r.reduce (amount, amount);
    }

    /** Returns a reduced shape at same center.
        @param amount How much to reduce by
    */
    Rectangle<Val> smaller (Val amount) const noexcept {
        auto s = *this;
        return s.reduce (-amount, -amount);
    }

    /** Returns a reduced shape at same center.
        @param amount How much to reduce by
    */
    Rectangle<Val> smaller (Val ax, Val ay) const noexcept {
        auto s = *this;
        return s.reduce (-ax, -ay);
    }

    /** Slice a region from the top leaving bottom */
    Rectangle<Val> slice_top (Val amount) noexcept {
        auto s = *this;
        s.height = amount;
        y += amount;
        height -= amount;
        return s;
    }

    /** Slice a region from the left leaving the right side */
    Rectangle<Val> slice_left (Val amount) noexcept {
        auto s = *this;
        s.width = amount;
        x += amount;
        width -= amount;
        return s;
    }

    /** Slice a region from the bottom leaving the top */
    Rectangle<Val> slice_bottom (Val amount) noexcept {
        auto s = *this;
        s.y = y + s.height - amount;
        s.height = amount;
        height -= amount;
        return s;
    }

    /** Slice a region from the right leaving the left side */
    Rectangle<Val> slice_right (Val amount) noexcept {
        auto s = *this;
        s.x = x + width - amount;
        s.width = amount;
        width -= amount;
        return s;
    }
};

} // namespace lvtk
