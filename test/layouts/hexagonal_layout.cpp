//
// Created by marcel on 15.09.21.
//

#include "catch.hpp"

#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/traits.hpp>

using namespace fiction;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wconversion"
#endif

TEST_CASE("Traits", "[hexagonal-layout]")
{
    using layout = hexagonal_layout<cartesian::ucoord_t, even_column>;

    //    CHECK(has_north_v<layout>);
    //    CHECK(has_east_v<layout>);
    //    CHECK(has_south_v<layout>);
    //    CHECK(has_west_v<layout>);
    //    CHECK(has_cardinal_checks_v<layout>);
    //    CHECK(has_above_v<layout>);
    //    CHECK(has_below_v<layout>);
    //    CHECK(has_elevation_checks_v<layout>);
    CHECK(is_coordinate_layout_v<layout>);

    CHECK(has_foreach_coordinate_v<layout>);
    CHECK(has_foreach_adjacent_coordinate_v<layout>);
}

template <typename Lyt>
void check_identity_conversion()
{
    Lyt layout{aspect_ratio<Lyt>{10, 10}};

    layout.foreach_coordinate([&layout](const auto& coord)
                              { CHECK(layout.to_offset_coordinate(layout.to_cube_coordinate(coord)) == coord); });
}

TEST_CASE("Coordinate conversions", "[hexagonal-layout]")
{
    using even_column_layout = hexagonal_layout<offset::ucoord_t, even_column>;
    using odd_column_layout  = hexagonal_layout<offset::ucoord_t, odd_column>;
    using even_row_layout    = hexagonal_layout<offset::ucoord_t, even_row>;
    using odd_row_layout     = hexagonal_layout<offset::ucoord_t, odd_row>;

    check_identity_conversion<even_column_layout>();
    check_identity_conversion<odd_column_layout>();
    check_identity_conversion<even_row_layout>();
    check_identity_conversion<odd_row_layout>();
}

template <typename Lyt>
void check_visited_coordinates()
{
    aspect_ratio<Lyt> ar{9, 9, 1};

    Lyt layout{ar};

    std::set<coordinate<Lyt>> visited{};

    const auto check1 = [&visited, &ar](const auto& t)
    {
        CHECK(t <= ar);

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.coordinates()) { check1(t); }
    CHECK(visited.size() == 200);

    visited.clear();

    layout.foreach_coordinate(check1);
    CHECK(visited.size() == 200);

    visited.clear();

    aspect_ratio<Lyt> ar_ground{ar.x, ar.y, 0};

    const auto check2 = [&visited, &ar_ground](const auto& t)
    {
        // iteration stays in ground layer
        CHECK(t.z == 0);
        CHECK(t <= ar_ground);

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.ground_coordinates()) { check2(t); }
    CHECK(visited.size() == 100);

    visited.clear();

    layout.foreach_ground_coordinate(check2);
    CHECK(visited.size() == 100);

    visited.clear();

    coordinate<Lyt> start{2, 2}, stop{5, 4};

    const auto check3 = [&visited, &start, &stop](const auto& t)
    {
        CHECK(t.z == 0);
        // iteration stays in between the bounds
        CHECK(t >= start);
        CHECK(t < stop);

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.coordinates(start, stop)) { check3(t); }
    CHECK(visited.size() == 23);

    visited.clear();

    layout.foreach_coordinate(check3, start, stop);
    CHECK(visited.size() == 23);
}

TEST_CASE("Coordinate iteration", "[hexagonal-layout]")
{
    using odd_row_layout     = hexagonal_layout<offset::ucoord_t, odd_row>;
    using even_row_layout    = hexagonal_layout<offset::ucoord_t, even_row>;
    using odd_column_layout  = hexagonal_layout<offset::ucoord_t, odd_column>;
    using even_column_layout = hexagonal_layout<offset::ucoord_t, even_column>;

    check_visited_coordinates<odd_row_layout>();
    check_visited_coordinates<even_row_layout>();
    check_visited_coordinates<odd_column_layout>();
    check_visited_coordinates<even_column_layout>();
}

TEST_CASE("Coordinate adjacencies", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_row>;

        layout lyt{{2, 2}};

        const std::set<coordinate<layout>> adj_00{{{0, 1}, {1, 0}}};
        CHECK(adj_00 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({0, 0}));

        const std::set<coordinate<layout>> adj_01{{{0, 0}, {1, 0}, {1, 1}, {0, 2}, {1, 2}}};
        CHECK(adj_01 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({0, 1}));

        const std::set<coordinate<layout>> adj_22{{{1, 2}, {1, 1}, {2, 1}}};
        CHECK(adj_22 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({2, 2}));
    }
    SECTION("even row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_row>;

        layout lyt{{2, 2}};

        const std::set<coordinate<layout>> adj_00{{{0, 1}, {1, 0}, {1, 1}}};
        CHECK(adj_00 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({0, 0}));

        const std::set<coordinate<layout>> adj_01{{{0, 0}, {1, 1}, {0, 2}}};
        CHECK(adj_01 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({0, 1}));

        const std::set<coordinate<layout>> adj_22{{{1, 2}, {2, 1}}};
        CHECK(adj_22 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({2, 2}));
    }
    SECTION("odd column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_column>;

        layout lyt{{2, 2}};

        const std::set<coordinate<layout>> adj_00{{{0, 1}, {1, 0}}};
        CHECK(adj_00 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({0, 0}));

        const std::set<coordinate<layout>> adj_01{{{0, 0}, {1, 0}, {0, 2}, {1, 1}}};
        CHECK(adj_01 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({0, 1}));

        const std::set<coordinate<layout>> adj_22{{{1, 1}, {2, 1}, {1, 2}}};
        CHECK(adj_22 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({2, 2}));
    }
    SECTION("even column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_column>;

        layout lyt{{2, 2}};

        const std::set<coordinate<layout>> adj_00{{{0, 1}, {1, 0}, {1, 1}}};
        CHECK(adj_00 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({0, 0}));

        const std::set<coordinate<layout>> adj_01{{{0, 0}, {1, 1}, {1, 2}, {0, 2}}};
        CHECK(adj_01 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({0, 1}));

        const std::set<coordinate<layout>> adj_22{{{1, 2}, {2, 1}}};
        CHECK(adj_22 == lyt.template adjacent_coordinates<std::set<coordinate<layout>>>({2, 2}));
    }
}