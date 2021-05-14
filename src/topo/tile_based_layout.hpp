//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include "coordinate.hpp"
#include "range.h"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <cstdint>

namespace fiction
{

class tile_based_layout
{
  public:
#pragma region Types and constructors

    using tile         = coord_t;
    using aspect_ratio = coord_t;

    struct tile_based_layout_storage
    {
        explicit tile_based_layout_storage(const aspect_ratio& ar) noexcept : dimension{ar} {};

        aspect_ratio dimension;
    };

    using base_type = tile_based_layout;

    using storage = std::shared_ptr<tile_based_layout_storage>;

    explicit tile_based_layout(const aspect_ratio& aspect_ratio) :
            strg{std::make_shared<tile_based_layout_storage>(aspect_ratio)}
    {}

    explicit tile_based_layout(std::shared_ptr<tile_based_layout_storage> storage) : strg{std::move(storage)} {}

#pragma endregion

#pragma region Structural properties

    [[nodiscard]] uint64_t x() const noexcept
    {
        return strg->dimension.x;
    }

    [[nodiscard]] uint64_t y() const noexcept
    {
        return strg->dimension.y;
    }

    [[nodiscard]] uint64_t z() const noexcept
    {
        return strg->dimension.z;
    }

    [[nodiscard]] uint64_t area() const noexcept
    {
        return x() * y();
    }

#pragma endregion

#pragma region cardinal operations

    [[nodiscard]] static constexpr tile north(const tile& t) noexcept
    {
        if (t.y == 0ull)
            return t;

        auto nt = t;
        --nt.y;

        return nt;
    }

    [[nodiscard]] tile east(const tile& t) const noexcept
    {
        auto et = t;

        if (t.x > x())
            et.d = 1;
        else if (t.x < x())
            ++et.x;

        return et;
    }

    [[nodiscard]] tile south(const tile& t) const noexcept
    {
        auto st = t;

        if (t.y > y())
            st.d = 1;
        else if (t.y < y())
            ++st.y;

        return st;
    }

    [[nodiscard]] static constexpr tile west(const tile& t) noexcept
    {
        if (t.x == 0ull)
            return t;

        auto wt = t;
        --wt.x;

        return wt;
    }

    [[nodiscard]] tile above(const tile& t) const noexcept
    {
        auto at = t;

        if (t.z > z())
            at.d = 1;
        else if (t.z < z())
            ++at.z;

        return at;
    }

    [[nodiscard]] static constexpr tile below(const tile& t) noexcept
    {
        if (t.z == 0ull)
            return t;

        auto bt = t;
        --bt.z;

        return bt;
    }

    [[nodiscard]] static constexpr bool is_northern_border(const tile& t) noexcept
    {
        return t.y == 0ull;
    }

    [[nodiscard]] bool is_eastern_border(const tile& t) const noexcept
    {
        return t.x == x();
    }

    [[nodiscard]] bool is_southern_border(const tile& t) const noexcept
    {
        return t.y == y();
    }

    [[nodiscard]] static constexpr bool is_western_border(const tile& t) noexcept
    {
        return t.x == 0ull;
    }

    [[nodiscard]] bool is_border(const tile& t) const noexcept
    {
        return is_northern_border(t) || is_eastern_border(t) || is_southern_border(t) || is_western_border(t);
    }

    [[nodiscard]] static constexpr bool is_ground_layer(const tile& t) noexcept
    {
        return t.z == 0ull;
    }

    [[nodiscard]] static constexpr bool is_crossing_layer(const tile& t) noexcept
    {
        return t.z > 0ull;
    }

#pragma endregion

#pragma region iterators

    [[nodiscard]] auto tiles(const tile& start = {}, const tile& stop = {}) const
    {
        return range_t{std::make_pair(coord_iterator{strg->dimension, start}, coord_iterator{strg->dimension, stop})};
    }

    template <typename Fn>
    void foreach_tile(Fn&& fn, const tile& start = {}, const tile& stop = {}) const
    {
        mockturtle::detail::foreach_element(coord_iterator{strg->dimension, start},
                                            coord_iterator{strg->dimension, stop}, fn);
    }

    [[nodiscard]] auto ground_tiles(const tile& start = {}, const tile& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio{x(), y(), 1};

        return range_t{std::make_pair(coord_iterator{ground_layer, start}, coord_iterator{ground_layer, stop})};
    }

    template <typename Fn>
    void foreach_ground_tile(Fn&& fn, const tile& start = {}, const tile& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio{x(), y(), 1};

        mockturtle::detail::foreach_element(coord_iterator{ground_layer, start}, coord_iterator{ground_layer, stop},
                                            fn);
    }

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_TILE_BASED_LAYOUT_HPP
