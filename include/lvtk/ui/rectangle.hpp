// Copyright 2022 Michael Fisher <mfisher@kushview.net>
// SPDX-License-Identifier: ISC

#pragma once

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

    std::string str() const noexcept {
        std::stringstream st;
        st << x << " " << y;
        return st.str();
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

    /** Returns a rectangle with top-left at x y */
    Rectangle<Val> at (Val x, Val y) const noexcept {
        return { Val(), Val(), width, height };
    }

    std::string str() const noexcept {
        std::stringstream out;
        out << x << " " << y << " " << width << " " << height;
        return out.str();
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
};

} // namespace lvtk
