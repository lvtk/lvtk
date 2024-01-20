// Copyright 2022 Michael Fisher <mfisher@lvtk.org>
// SPDX-License-Identifier: ISC

#pragma once

#include <algorithm>

#include <lvtk/string.hpp>
#include <lvtk/ui/point.hpp>

namespace lvtk {

/** A rectangle. x y width height.
    @tparam Val the value type of this rectangle.
    @ingroup graphics
    @headerfile lvtk/ui/rectangle.hpp
*/
template <typename Val>
struct Rectangle {
    /** X Coordinate */
    Val x {};
    /** Y Coordinate */
    Val y {};
    /** Width */
    Val width {};
    /** Height */
    Val height {};

    /** Makes an empty rectangle */
    Rectangle() = default;

    /** Make a new rectangle at x=0 and y=0
        @param w Width
        @param h Height
    */
    Rectangle (Val w, Val h)
        : x (0), y (0), width (w), height (h) {}

    /** Make a new rectangle
        @param xc X coord
        @param yc Y coord
        @param w Width
        @param h Height
    */
    Rectangle (Val xc, Val yc, Val w, Val h)
        : x (xc), y (yc), width (w), height (h) {}

    /** Get the pos (x/y) of this rect 
        @returns a Point with xy coords
    */
    Point<Val> pos() const noexcept { return { x, y }; }

    /** Empty test.
        @returns true if width or height <= 0
    */
    bool empty() const noexcept { return width <= Val() || height <= Val(); }

    /** convert to a differnt value type
        @tparam T the other value type to convert to
        @returns a rectangle converted to new value `T`
    */
    template <typename T>
    Rectangle<T> as() const noexcept {
        return {
            static_cast<T> (x),
            static_cast<T> (y),
            static_cast<T> (width),
            static_cast<T> (height)
        };
    }

    /** Returns a copy of this shape with top-left at x y 
        @param x New x-coord
        @param y New y-coord
    */
    Rectangle at (Val x, Val y) const noexcept {
        return { x, y, width, height };
    }

    /** Returns a copy of this shape with top-left at xy.
        @param xy New x and y coordinate
    */
    constexpr Rectangle at (Val xy) const noexcept {
        return { xy, xy, width, height };
    }

    /** Convert to a string
        @returns A formatted string "x y width height"
    */
    std::string str() const noexcept {
        String out;
        out << x << " " << y << " " << width << " " << height;
        return out;
    }

    /** Returns true if point x1 y1 is contained
        @param x1 x coord
        @param y1 y coord
    */
    bool contains (Val x1, Val y1) const noexcept {
        return x1 >= x && y1 >= y && x1 < x + width && y1 < y + height;
    }

    /** Returns true if point x1 y1 is contained
        @param pt Coordinate to check
    */
    bool contains (Point<Val> pt) const noexcept {
        return pt.x >= x && pt.y >= y
               && pt.x < x + width && pt.y < y + height;
    }

    /** Returns true if the other rectangle is contained
        @param other Other rect to check.
    */
    bool contains (Rectangle other) const noexcept {
        return x <= other.x && y <= other.y
               && x + width >= other.x + other.width
               && y + height >= other.y + other.height;
    }

    bool operator== (const Rectangle& o) const noexcept {
        return x == o.x && y == o.y && width == o.width && height == o.height;
    }

    bool operator!= (const Rectangle& o) const noexcept {
        return x != o.x || y != o.y || width != o.width || height != o.height;
    }

    Rectangle operator+ (Point<Val> delta) const noexcept {
        return { x + delta.x, y + delta.y };
    }
    Rectangle& operator+= (Point<Val> delta) noexcept {
        x += delta.x;
        y += delta.y;
        return *this;
    }
    Rectangle operator- (Point<Val> delta) const noexcept {
        return { x - delta.x, y - delta.y };
    }
    Rectangle& operator-= (Point<Val> delta) noexcept {
        x -= delta.x;
        y -= delta.y;
        return *this;
    }

    template <typename F>
    Rectangle& operator*= (F scale) noexcept {
        x *= (Val) scale;
        y *= (Val) scale;
        width *= (Val) scale;
        height *= (Val) scale;
        return *this;
    }

    template <typename F>
    Rectangle operator* (F scale) const noexcept {
        Rectangle r (*this);
        r *= scale;
        return r;
    }

    template <typename F>
    Rectangle& operator/= (F scale) noexcept {
        x /= scale;
        y /= scale;
        width /= scale;
        height /= scale;
        return *this;
    }

    template <typename F>
    Rectangle operator/ (F scale) const noexcept {
        Rectangle r (*this);
        r /= scale;
        return r;
    }

    /** Reduce this rectangle by delta dx and dy
        @param dx Delta x
        @param dy Detla y
    */
    Rectangle& reduce (Val dx, Val dy) {
        x += dx;
        y += dy;
        width -= (dx * 2);
        height -= (dy * 2);
        return *this;
    }

    /** Reduce x and y by delta
        @param delta Amount to reduce by
    */
    Rectangle& reduce (Val delta) {
        return reduce (delta, delta);
    }

    /** Returns an expanded shape at same center.
        @param amount How much to grow by.
    */
    Rectangle bigger (Val amount) const noexcept {
        auto r = *this;
        return r.reduce (-amount, -amount);
    }

    /** Returns a reduced shape at same center.
        @param amount How much to reduce by
    */
    Rectangle smaller (Val amount) const noexcept {
        auto s = *this;
        return s.reduce (amount, amount);
    }

    /** Returns a reduced shape at same center.
        @param ax How much to reduce the x-axis by
        @param ay How much to reduce the y-axis by
    */
    Rectangle smaller (Val ax, Val ay) const noexcept {
        auto s = *this;
        return s.reduce (ax, ay);
    }

    /** Slice a region from the top leaving bottom
        @param amount Amount to slice by
    */
    Rectangle slice_top (Val amount) noexcept {
        auto s   = *this;
        s.height = amount;
        y += amount;
        height -= amount;
        return s;
    }

    /** Slice a region from the left leaving the right side
        @param amount Amount to slice by
    */
    Rectangle slice_left (Val amount) noexcept {
        auto s  = *this;
        s.width = amount;
        x += amount;
        width -= amount;
        return s;
    }

    /** Slice a region from the bottom leaving the top
        @param amount Amount to slice by
    */
    Rectangle slice_bottom (Val amount) noexcept {
        auto s   = *this;
        s.y      = y + s.height - amount;
        s.height = amount;
        height -= amount;
        return s;
    }

    /** Slice a region from the right leaving the left side
        @param amount Amount to slice by
    */
    Rectangle slice_right (Val amount) noexcept {
        auto s  = *this;
        s.x     = x + width - amount;
        s.width = amount;
        width -= amount;
        return s;
    }

    /** Returns true if this rect intersects the other
        @param o The other rect
    */
    bool intersects (Rectangle o) const noexcept {
        return x + width > o.x
               && y + height > o.y
               && x < o.x + o.width
               && y < o.y + o.height
               && width > Val() && height > Val()
               && o.width > Val() && o.height > Val();
    }

    /** Returns the intersection with another rectangle
        @param o The other rect
    */
    Rectangle intersection (Rectangle o) const noexcept {
        Rectangle r;
        r.x      = std::max (x, o.x);
        r.y      = std::max (y, o.y);
        r.width  = std::min (x + width, o.x + o.width) - r.x;
        r.height = std::min (y + height, o.y + o.height) - r.y;
        return r.height >= Val() && r.width >= Val() ? r : Rectangle();
    }
};

} // namespace lvtk
