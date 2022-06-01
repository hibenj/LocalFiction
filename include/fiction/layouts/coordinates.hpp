//
// Created by marcel on 01.05.21.
//

#ifndef FICTION_COORDINATES_HPP
#define FICTION_COORDINATES_HPP

#include <fmt/format.h>

#include <cstdint>
#include <functional>
#include <iostream>

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wconversion"

namespace fiction
{

/**
 * Provides offset coordinates. An offset coordinate is a coordinate that defines a location via an offset from a fixed
 * point (origin). Cartesian coordinates are offset coordinates.
 */
namespace offset
{
/**
 * Unsigned offset coordinates. The implementation is optimized for memory-efficiency and fits within 64 bits.
 * Coordinates span from (0, 0, 0) to (2^31 - 1, 2^31 - 1, 1).
 * Each coordinate has a dead indicator that can be used to represent that it is not in use.
 */
struct ucoord_t
{
    /**
     * MSB acts as dead indicator.
     */
    uint64_t d : 1;
    /**
     * 1 bit for the z coordinate.
     */
    uint64_t z : 1;
    /**
     * 31 bit for the y coordinate.
     */
    uint64_t y : 31;
    /**
     * 31 bit for the x coordinate.
     */
    uint64_t x : 31;

    /**
     * Default constructor. Creates a dead coordinate at (0, 0, 0).
     */
    constexpr ucoord_t() noexcept :
            d{static_cast<decltype(d)>(1u)},  // default-constructed ucoord_ts are dead
            z{static_cast<decltype(z)>(0u)},
            y{static_cast<decltype(y)>(0u)},
            x{static_cast<decltype(x)>(0u)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @tparam Z Type of z.
     * @param x_ x position.
     * @param y_ y position.
     * @param z_ z position.
     */
    template <class X, class Y, class Z>
    constexpr ucoord_t(X x_, Y y_, Z z_) noexcept :
            d{static_cast<decltype(d)>(0u)},
            z{static_cast<decltype(z)>(z_)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, 0).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @param x_ x position.
     * @param y_ y position.
     */
    template <class X, class Y>
    constexpr ucoord_t(X x_, Y y_) noexcept :
            d{static_cast<decltype(d)>(0u)},
            z{static_cast<decltype(z)>(0u)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}
    /**
     * Standard constructor. Instantiates a coordinate from an uint64_t, where the positions are encoded in the
     * following four parts of the unsigned 64-bit integer (from MSB to LSB):
     *  - 1 bit for the dead indicator
     *  - 1 bit for the z position
     *  - 31 bit for the y position
     *  - 31 bit for the x position
     *
     * @param t Unsigned 64-bit integer to instantiate the coordinate from.
     */
    constexpr explicit ucoord_t(const uint64_t t) noexcept :
            d{static_cast<decltype(d)>(t >> 63)},
            z{static_cast<decltype(z)>((t << 1) >> 63)},
            y{static_cast<decltype(y)>((t << 2) >> 33)},
            x{static_cast<decltype(x)>((t << 33) >> 33)}
    {}
    /**
     * Allows explicit conversion to uint64_t. Segments an unsigned 64-bit integer into four parts (from MSB to LSB):
     *  - 1 bit for the dead indicator
     *  - 1 bit for the z position
     *  - 31 bit for the y position
     *  - 31 bit for the x position
     */
    explicit constexpr operator uint64_t() const noexcept
    {
        return (((((((0ull | d) << 1) | z) << 31) | y) << 31) | x);
    }
    /**
     * Returns whether the coordinate is dead.
     *
     * @return True iff coordinate is dead.
     */
    [[nodiscard]] constexpr bool is_dead() const noexcept
    {
        return static_cast<bool>(d);
    }
    /**
     * Returns a dead copy of the coordinate, i.e., (1, x, y, z).
     *
     * @return A dead copy of the coordinate.
     */
    [[nodiscard]] constexpr ucoord_t get_dead() const noexcept
    {
        return ucoord_t{static_cast<uint64_t>(*this) | static_cast<uint64_t>(ucoord_t{})};
    }
    /**
     * Compares against another coordinate for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return True iff both coordinates are identical.
     */
    constexpr bool operator==(const ucoord_t& other) const noexcept
    {
        return d == other.d && z == other.z && y == other.y && x == other.x;
    }
    /**
     * Compares against another coordinate's uint64_t representation for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate representation in uint64_t format.
     * @return True iff this coordinate is equal to the converted one.
     */
    constexpr bool operator==(const uint64_t& other) const noexcept
    {
        return static_cast<uint64_t>(*this) == other;
    }
    /**
     * Compares against another coordinate for inequality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return True iff both coordinates are not identical.
     */
    constexpr bool operator!=(const ucoord_t& other) const noexcept
    {
        return !(*this == other);
    }
    /**
     * Determine whether this coordinate is "less than" another one. This is the case if z is smaller, or if z is equal
     * but y is smaller, or if z and y are equal but x is smaller.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "less than" the other coordinate.
     */
    constexpr bool operator<(const ucoord_t& other) const noexcept
    {
        if (z < other.z)
            return true;

        if (z == other.z)
        {
            if (y < other.y)
                return true;

            if (y == other.y)
            {
                return x < other.x;
            }
        }

        return false;
    }
    /**
     * Determine whether this coordinate is "greater than" another one. This is the case if the other one is "less
     * than".
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "greater than" the other coordinate.
     */
    constexpr bool operator>(const ucoord_t& other) const noexcept
    {
        return other < *this;
    }
    /**
     * Determine whether this coordinate is "less than or equal to" another one. This is the case if this one is not
     * "greater than" the other.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "less than or equal to" the other coordinate.
     */
    constexpr bool operator<=(const ucoord_t& other) const noexcept
    {
        return !(*this > other);
    }
    /**
     * Determine whether this coordinate is "greater than or equal to" another one. This is the case if this one is not
     * "less than" the other.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "greater than or equal to" the other coordinate.
     */
    constexpr bool operator>=(const ucoord_t& other) const noexcept
    {
        return !(*this < other);
    }
    /**
     * Returns a string representation of the coordinate of the form "(x, y, z)" that does not respect the dead
     * indicator.
     *
     * @return String representation of the form "(x, y, z)".
     */
    [[nodiscard]] std::string str() const noexcept
    {
        return fmt::format("({},{},{})", x, y, z);
    }
};

inline std::ostream& operator<<(std::ostream& os, const ucoord_t& t)
{
    os << t.str();
    return os;
}

/**
 * An iterator type that allows to enumerate coordinates in order within a boundary.
 *
 * @tparam CoordinateType Type of coordinate to enumerate.
 */
template <typename CoordinateType>
class coord_iterator
{
  public:
    using value_type = CoordinateType;
    /**
     * Standard constructor. Initializes the iterator with a starting position and the boundary within to enumerate.
     *
     * With dimension = (1, 2, 1) and start = (0, 0, 0), the following order would be enumerated:
     *
     * - (0, 0, 0)
     * - (1, 0, 0)
     * - (0, 1, 0)
     * - (1, 1, 0)
     * - (0, 2, 0)
     * - (1, 2, 0)
     * - (0, 0, 1)
     * - (1, 0, 1)
     * - (0, 1, 1)
     * - (1, 1, 1)
     * - (0, 2, 1)
     * - (1, 2, 1)
     *
     * coord_iterator is compatible with the STL forward_iterator category.
     *
     * @param dimension Boundary within to enumerate. Iteration wraps at its limits.
     * @param start Starting coordinate to enumerate first.
     */
    constexpr explicit coord_iterator(const CoordinateType& dimension, const CoordinateType& start) noexcept :
            aspect_ratio{dimension},
            coord{start}
    {}
    /**
     * Increments the iterator.
     *
     * @return Reference to the incremented iterator.
     */
    constexpr coord_iterator& operator++() noexcept
    {
        if (coord != aspect_ratio)
        {
            ++coord.x;

            if (coord.x > aspect_ratio.x)
            {
                coord.x = 0;

                ++coord.y;
                if (coord.y > aspect_ratio.y)
                {
                    coord.y = 0;

                    ++coord.z;
                }
            }
        }
        else
        {
            coord = coord.get_dead();
        }

        return *this;
    }

    constexpr const coord_iterator operator++(int) noexcept
    {
        const auto result{*this};

        ++(*this);

        return result;
    }

    constexpr CoordinateType operator*() const noexcept
    {
        return coord;
    }

    constexpr bool operator==(const coord_iterator& other) const noexcept
    {
        return (coord == other.coord);
    }

    constexpr bool operator!=(const coord_iterator& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const coord_iterator& other) const noexcept
    {
        return (coord < other.coord);
    }

    constexpr bool operator<=(const coord_iterator& other) const noexcept
    {
        return (coord <= other.coord);
    }

  private:
    const CoordinateType aspect_ratio;

    CoordinateType coord;
};
}  // namespace offset

/**
 * Provides cube coordinates. Cube coordinates are used as a way to identify faces in a hexagonal grid. A wonderful
 * resource on the topic is: https://www.redblobgames.com/grids/hexagons/#coordinates-cube
 */
namespace cube
{
/**
 * Signed cube coordinates.
 *
 * Each coordinate has a dead indicator that can be used to represent that it is not in use.
 */
struct coord_t
{
    /**
     * Dead indicator.
     */
    bool d;
    /**
     * z coordinate.
     */
    int32_t z;
    /**
     * y coordinate.
     */
    int32_t y;
    /**
     * x coordinate.
     */
    int32_t x;

    /**
     * Default constructor. Creates a dead coordinate at (0, 0, 0).
     */
    constexpr coord_t() noexcept :
            d{true},  // default-constructed coord_ts are dead
            z{static_cast<decltype(z)>(0)},
            y{static_cast<decltype(y)>(0)},
            x{static_cast<decltype(x)>(0)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @tparam Z Type of z.
     * @param x_ x position.
     * @param y_ y position.
     * @param z_ z position.
     */
    template <class X, class Y, class Z>
    constexpr coord_t(X x_, Y y_, Z z_) noexcept :
            d{false},
            z{static_cast<decltype(z)>(z_)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, 0).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @param x_ x position.
     * @param y_ y position.
     */
    template <class X, class Y>
    constexpr coord_t(X x_, Y y_) noexcept :
            d{false},
            z{static_cast<decltype(z)>(0)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}
    /**
     * Returns whether the coordinate is dead.
     *
     * @return True iff coordinate is dead.
     */
    [[nodiscard]] constexpr bool is_dead() const noexcept
    {
        return static_cast<bool>(d);
    }
    /**
     * Returns a dead copy of the coordinate, i.e., (1, x, y, z).
     *
     * @return A dead copy of the coordinate.
     */
    [[nodiscard]] constexpr coord_t get_dead() const noexcept
    {
        auto dead_coord{*this};
        dead_coord.d = true;
        return dead_coord;
    }
    /**
     * Compares against another coordinate for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return True iff both coordinates are identical.
     */
    constexpr bool operator==(const coord_t& other) const noexcept
    {
        return d == other.d && z == other.z && y == other.y && x == other.x;
    }
    /**
     * Compares against another coordinate for inequality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return True iff both coordinates are not identical.
     */
    constexpr bool operator!=(const coord_t& other) const noexcept
    {
        return !(*this == other);
    }
    /**
     * Determine whether this coordinate is "less than" another one. This is the case if z is smaller, or if z is equal
     * but y is smaller, or if z and y are equal but x is smaller.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "less than" the other coordinate.
     */
    constexpr bool operator<(const coord_t& other) const noexcept
    {
        if (z < other.z)
            return true;

        if (z == other.z)
        {
            if (y < other.y)
                return true;

            if (y == other.y)
            {
                return x < other.x;
            }
        }

        return false;
    }
    /**
     * Determine whether this coordinate is "greater than" another one. This is the case if the other one is "less
     * than".
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "greater than" the other coordinate.
     */
    constexpr bool operator>(const coord_t& other) const noexcept
    {
        return other < *this;
    }
    /**
     * Determine whether this coordinate is "less than or equal to" another one. This is the case if this one is not
     * "greater than" the other.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "less than or equal to" the other coordinate.
     */
    constexpr bool operator<=(const coord_t& other) const noexcept
    {
        return !(*this > other);
    }
    /**
     * Determine whether this coordinate is "greater than or equal to" another one. This is the case if this one is not
     * "less than" the other.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "greater than or equal to" the other coordinate.
     */
    constexpr bool operator>=(const coord_t& other) const noexcept
    {
        return !(*this < other);
    }
    /**
     * Adds another coordinate to this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to add.
     * @return Sum of both coordinates.
     */
    constexpr coord_t operator+(const coord_t& other) const noexcept
    {
        return coord_t{x + other.x, y + other.y, z + other.z};
    }
    /**
     * Subtracts another coordinate from this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to subtract.
     * @return Difference of both coordinates.
     */
    constexpr coord_t operator-(const coord_t& other) const noexcept
    {
        return coord_t{x - other.x, y - other.y, z - other.z};
    }
    /**
     * Returns a string representation of the coordinate of the form "(x, y, z)" that does not respect the dead
     * indicator.
     *
     * @return String representation of the form "(x, y, z)".
     */
    [[nodiscard]] std::string str() const noexcept
    {
        return fmt::format("({},{},{})", x, y, z);
    }
};

}  // namespace cube

/**
 * Computes the area of a given coordinate assuming its origin is (0, 0, 0). Calculates (x + 1) * (y + 1).
 *
 * @tparam CoordinateType Coordinate type.
 * @param coord Coordinate.
 * @return Area of coord.
 */
template <typename CoordinateType>
uint64_t area(const CoordinateType& coord) noexcept
{
    return (coord.x + static_cast<decltype(coord.x)>(1)) * (coord.y + static_cast<decltype(coord.y)>(1));
}
/**
 * Computes the volume of a given coordinate assuming its origin is (0, 0, 0). Calculates (x + 1) * (y + 1) * (z + 1).
 *
 * @tparam CoordinateType Coordinate type.
 * @param coord Coordinate.
 * @return Volume of coord.
 */
template <typename CoordinateType>
uint64_t volume(const CoordinateType& coord) noexcept
{
    return (coord.x + static_cast<decltype(coord.x)>(1)) * (coord.y + static_cast<decltype(coord.y)>(1)) *
           (coord.z + static_cast<decltype(coord.z)>(1));
}

}  // namespace fiction

namespace std
{
// define std::hash overload for offset::ucoord_t
template <>
struct hash<fiction::offset::ucoord_t>
{
    std::size_t operator()(const fiction::offset::ucoord_t& c) const noexcept
    {
        return static_cast<std::size_t>(std::hash<uint64_t>{}(static_cast<uint64_t>(c)));
    }
};
// define std::hash overload for cube::coord_t
template <>
struct hash<fiction::cube::coord_t>
{
    // based on: https://stackoverflow.com/questions/25649342/hash-function-for-3d-integer-coordinates
    std::size_t operator()(const fiction::cube::coord_t& c) const noexcept
    {
        return static_cast<std::size_t>((c.x * 18397ll) + (c.y * 20483ll) + (c.z * 29303ll) + static_cast<int>(c.d));
    }
};
// make offset::coord_iterator compatible with STL iterator categories
template <typename Coordinate>
struct iterator_traits<fiction::offset::coord_iterator<Coordinate>>
{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Coordinate;
};
}  // namespace std

namespace fmt
{
// make offset::ucoord_t compatible with fmt::format
template <>
struct formatter<fiction::offset::ucoord_t>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::offset::ucoord_t& c, FormatContext& ctx)
    {
        return format_to(ctx.out(), "({},{},{})", c.x, c.y, c.z);
    }
};
// make cube::coord_t compatible with fmt::format
template <>
struct formatter<fiction::cube::coord_t>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::cube::coord_t& c, FormatContext& ctx)
    {
        return format_to(ctx.out(), "({},{},{})", c.x, c.y, c.z);
    }
};
}  // namespace fmt

#pragma GCC diagnostic pop

#endif  // FICTION_COORDINATES_HPP
