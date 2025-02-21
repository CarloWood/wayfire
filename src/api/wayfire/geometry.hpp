#ifndef WF_GEOMETRY_HPP
#define WF_GEOMETRY_HPP

#include "wayfire/maximization.hpp"
#include <ostream>
#include <wayfire/nonstd/wlroots.hpp>

namespace wf
{
struct point_t
{
    int x, y;
};

struct pointf_t
{
    double x, y;

    pointf_t() : x(0), y(0)
    {}
    pointf_t(double _x, double _y) : x(_x), y(_y)
    {}
    explicit pointf_t(const point_t& pt) : x(pt.x), y(pt.y)
    {}

    pointf_t operator +(const pointf_t& other) const
    {
        return pointf_t{x + other.x, y + other.y};
    }

    pointf_t operator -(const pointf_t& other) const
    {
        return pointf_t{x - other.x, y - other.y};
    }

    pointf_t& operator +=(const pointf_t& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    pointf_t& operator -=(const pointf_t& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    pointf_t operator -() const
    {
        return pointf_t{-x, -y};
    }

    point_t round_down() const
    {
        return point_t{static_cast<int>(x), static_cast<int>(y)};
    }
};

struct dimensions_t
{
    int32_t width;
    int32_t height;
};

using geometry_t = wlr_box;

struct rectangle_t
{
    int x1, y1; // Top-left coorinate, inclusive.
    int x2, y2; // One pixel beyond the bottom-right (aka, not inclusive).

    // Default constructor.
    rectangle_t() = default;

    // Construct a rectangle_t from top-left and bottom-right coordinates.
    rectangle_t(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2)
    {}

    // Construct a rectangle_t from a geometry_t.
    rectangle_t(geometry_t g) : x1(g.x), y1(g.y), x2(g.x + g.width), y2(g.y + g.height)
    {}

    // We can't add a constructor to geometry_t because that must be an aggregate type.
    // Therefore add conversion from rectangle_t to geometry_t here.
    operator geometry_t() const {
        return {x1, y1, x2 - x1, y2 - y1};
    }
};

point_t origin(const geometry_t& geometry);
dimensions_t dimensions(const geometry_t& geometry);
geometry_t construct_box(
    const wf::point_t& origin, const wf::dimensions_t& dimensions);

/* Returns the intersection of the two boxes, if the boxes don't intersect,
 * the resulting geometry has undefined (x,y) and width == height == 0 */
geometry_t geometry_intersection(const geometry_t& r1,
    const geometry_t& r2);

std::ostream& operator <<(std::ostream& stream, const wf::rectangle_t& rectangle);
std::ostream& operator <<(std::ostream& stream, const wf::point_t& point);
std::ostream& operator <<(std::ostream& stream, const wf::pointf_t& pointf);
std::ostream& operator <<(std::ostream& stream, const wf::dimensions_t& dims);

bool operator ==(const wf::dimensions_t& a, const wf::dimensions_t& b);
bool operator !=(const wf::dimensions_t& a, const wf::dimensions_t& b);

bool operator ==(const wf::point_t& a, const wf::point_t& b);
bool operator !=(const wf::point_t& a, const wf::point_t& b);

wf::point_t operator +(const wf::point_t& a, const wf::point_t& b);
wf::point_t operator -(const wf::point_t& a, const wf::point_t& b);

wf::point_t operator -(const wf::point_t& a);

/** Return the closest valume to @value which is in [@min, @max] */
template<class T>
T clamp(T value, T min, T max)
{
    return std::min(std::max(value, min), max);
}

/**
 * Return the closest geometry to window which is completely inside the output.
 * The returned geometry might be smaller, but never bigger than window.
 */
geometry_t clamp(geometry_t window, geometry_t output);

// Transform a subbox from coordinate space A to coordinate space B.
// The returned subbox will occupy the same relative part of @B as
// @box occupies in @A.
wf::geometry_t scale_box(wf::geometry_t A, wf::geometry_t B, wf::geometry_t box);

/**
 *
 * Let the members of geometry_difference_t be an outwards displacement.
 *
 *                   from.x
 *                       |
 *              to.x     v
 *                |      +<-----from.width----->+
 *                v      |                      |
 *      to.y ->---┌─────────────────────────────────────┐--------------+
 *                │      |         ^ d.top      |       │              ^
 *    from.y ->---│------┌─────────┴────────────┐-------│----+         |
 *                │      │                      │       │    ^         |
 *                │d.left│                      │d.right│    |         |
 *                │<-----│                      │------>│  from.height |
 *                │      │                      │       │    |         |
 *                │      │                      │       │    v     to.height
 *                │      └─────────┬────────────┘-------│----+         |
 *                │                v d.bottom           │              v
 *                └─────────────────────────────────────┘--------------+
 *                |                                     |
 *                +<-------------to.width-------------->+
 *
 * geometry_t from;
 * geometry_difference_t d;
 * geometry_t to = from + d;
 *
 * In the above `from` is expanded, grown, in each of the four directions.
 * However, negative values of left, right, bottom and top are also legal;
 * in that case the corresponding border simply shifts the opposite way.
 *
 * It is not possible to add geometry_t's or to negate them, of course.
 * But it is possible to subtract them: d = to - from.
 */
struct geometry_difference_t
{
    int left;
    int right;
    int bottom;
    int top;
};

/**
 * Add a geometry_difference_t to a geometry_t.
 */
inline geometry_t operator +(geometry_t const& g, geometry_difference_t const& m)
{
    return {g.x - m.left, g.y - m.top, g.width + m.left + m.right, g.height + m.top + m.bottom};
}

/**
 * Subtract a geometry_difference_t from a geometry_t.
 */
inline geometry_t operator -(geometry_t const& g, geometry_difference_t const& m)
{
    return {g.x + m.left, g.y + m.top, g.width - m.left - m.right, g.height - m.top - m.bottom};
}

/**
 * Subtract a geometry_t from a geometry_t.
 */
inline geometry_difference_t operator -(geometry_t const& to, geometry_t const& from)
{
    return {.left = from.x - to.x, .right = to.x + to.width - (from.x + from.width),
        .bottom   = to.y + to.height - (from.y + from.height), .top = from.y - to.y};
}

/**
 * Conditionally expand (or shrink in case of negative values) each edge with
 * the displacement given by delta OR tiled_edge_delta, as a function of `tiled_edges`.
 */
inline geometry_t expand_geometry_if(geometry_t geometry_in,
    uint32_t tiled_edges,
    geometry_difference_t const& tiled_edge_delta,
    geometry_difference_t const& delta = {})
{
    rectangle_t rectangle{geometry_in};

    rectangle.x1 -= (tiled_edges & WLR_EDGE_LEFT) ? tiled_edge_delta.left : delta.left;
    rectangle.y1 -= (tiled_edges & WLR_EDGE_TOP) ? tiled_edge_delta.top : delta.top;
    rectangle.x2 += (tiled_edges & WLR_EDGE_RIGHT) ? tiled_edge_delta.right : delta.right;
    rectangle.y2 += (tiled_edges & WLR_EDGE_BOTTOM) ? tiled_edge_delta.bottom : delta.bottom;

    return rectangle;
}

/**
 *
 */
inline rectangle_t geometry_switch_if(uint32_t tiled_edges, rectangle_t const& g1, rectangle_t const& g0)
{
    return {
        (tiled_edges & WLR_EDGE_LEFT) ? g1.x1 : g0.x1,
        (tiled_edges & WLR_EDGE_TOP) ? g1.y1 : g0.y1,
        (tiled_edges & WLR_EDGE_RIGHT) ? g1.x2 : g0.x2,
        (tiled_edges & WLR_EDGE_BOTTOM) ? g1.y2 : g0.y2
    };
}

/**
 * Invert all displacements of a geometry_difference_t.
 */
inline geometry_difference_t operator -(geometry_difference_t const& delta)
{
    return {.left = -delta.left, .right = -delta.right, .bottom = -delta.bottom, .top = -delta.top};
}
} // namespace wf

bool operator ==(const wf::geometry_t& a, const wf::geometry_t& b);
bool operator !=(const wf::geometry_t& a, const wf::geometry_t& b);

wf::point_t operator +(const wf::point_t& a, const wf::geometry_t& b);
wf::geometry_t operator +(const wf::geometry_t & a, const wf::point_t& b);
wf::geometry_t operator -(const wf::geometry_t & a, const wf::point_t& b);

/** Scale the box */
wf::geometry_t operator *(const wf::geometry_t& box, double scale);

/* @return The length of the given vector */
double abs(const wf::point_t & p);

/* Returns true if point is inside rect */
bool operator &(const wf::geometry_t& rect, const wf::point_t& point);
/* Returns true if point is inside rect */
bool operator &(const wf::geometry_t& rect, const wf::pointf_t& point);
/* Returns true if the two geometries have a common point */
bool operator &(const wf::geometry_t& r1, const wf::geometry_t& r2);

/* Make geometry_t printable */
std::ostream& operator <<(std::ostream& stream, const wf::geometry_t& geometry);

#endif /* end of include guard: WF_GEOMETRY_HPP */
