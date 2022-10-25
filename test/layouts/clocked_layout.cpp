//
// Created by marcel on 31.03.21.
//

#include "catch.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/traits.hpp>

#include <set>

using namespace fiction;

TEST_CASE("Traits", "[clocked-layout]")
{
    using layout = clocked_layout<cartesian_layout<offset::ucoord_t>>;

    CHECK(has_is_incoming_clocked_v<layout>);
    CHECK(has_is_outgoing_clocked_v<layout>);
    CHECK(has_foreach_incoming_clocked_zone_v<layout>);
    CHECK(has_foreach_outgoing_clocked_zone_v<layout>);
}

TEST_CASE("Clocking", "[clocked-layout]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;

    clk_lyt layout{clk_lyt::aspect_ratio{1, 1, 0}, twoddwave_clocking<clk_lyt>()};

    SECTION("2DDWave Clocking")
    {
        CHECK(layout.is_clocking_scheme(clock_name::TWODDWAVE));
        CHECK(!layout.is_clocking_scheme(clock_name::RES));
        CHECK(layout.is_regularly_clocked());
        CHECK(layout.num_clocks() == 4);

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 1);
        CHECK(layout.get_clock_number({0, 1}) == 1);
        CHECK(layout.get_clock_number({1, 1}) == 2);

        CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
        CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {1, 1}));

        CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({1, 1}, {1, 1}));

        layout.assign_clock_number({1, 0}, 2);
        layout.assign_clock_number({0, 1}, 2);
        layout.assign_clock_number({1, 1}, 3);

        CHECK(!layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 2);
        CHECK(layout.get_clock_number({0, 1}) == 2);
        CHECK(layout.get_clock_number({1, 1}) == 3);

        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
        CHECK(!layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));

        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {0, 1}));
    }

    SECTION("Replace with USE")
    {
        layout.replace_clocking_scheme(use_clocking<clk_lyt>());

        CHECK(!layout.is_clocking_scheme(clock_name::TWODDWAVE));
        CHECK(layout.is_clocking_scheme(clock_name::USE));
        CHECK(layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 1);
        CHECK(layout.get_clock_number({0, 1}) == 3);
        CHECK(layout.get_clock_number({1, 1}) == 2);

        CHECK(layout.is_incoming_clocked({0, 1}, {1, 1}));
        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {1, 1}));

        CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {0, 0}));
        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {0, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({1, 1}, {1, 1}));

        layout.assign_clock_number({1, 0}, 2);
        layout.assign_clock_number({0, 1}, 2);
        layout.assign_clock_number({1, 1}, 3);

        CHECK(!layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 2);
        CHECK(layout.get_clock_number({0, 1}) == 2);
        CHECK(layout.get_clock_number({1, 1}) == 3);
    }
}

TEST_CASE("Iteration", "[clocked-layout]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;

    clk_lyt layout{clk_lyt::aspect_ratio{2, 2, 0}, twoddwave_clocking<clk_lyt>()};

    CHECK(layout.incoming_clocked_zones<std::set<clk_lyt::coordinate>>({0, 0}).empty());
    CHECK(layout.outgoing_clocked_zones<std::set<clk_lyt::coordinate>>({2, 2}).empty());

    auto s1 = layout.incoming_clocked_zones<std::set<clk_lyt::coordinate>>({1, 1});
    auto s2 = std::set<clk_lyt::coordinate>{{{1, 0}, {0, 1}}};

    CHECK(s1 == s2);

    layout.foreach_incoming_clocked_zone({1, 1}, [&s2](const auto& cz) { CHECK(s2.count(cz) > 0); });

    auto s3 = layout.outgoing_clocked_zones<std::set<clk_lyt::coordinate>>({1, 1});
    auto s4 = std::set<clk_lyt::coordinate>{{{1, 2}, {2, 1}}};

    layout.foreach_outgoing_clocked_zone({1, 1}, [&s4](const auto& cz) { CHECK(s4.count(cz) > 0); });

    CHECK(s3 == s4);

    layout.foreach_outgoing_clocked_zone({1, 1}, [&s4](const auto& cz) { CHECK(s4.count(cz) > 0); });
}

TEST_CASE("Structural properties", "[clocked-layout]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;

    SECTION("2DDWave Clocking")
    {
        clk_lyt layout{clk_lyt::aspect_ratio{2, 2, 0}, twoddwave_clocking<clk_lyt>()};

        CHECK(layout.in_degree({0, 0}) == static_cast<clk_lyt::degree_t>(0));
        CHECK(layout.in_degree({1, 0}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.in_degree({2, 0}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.in_degree({1, 1}) == static_cast<clk_lyt::degree_t>(2));

        CHECK(layout.out_degree({1, 1}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.out_degree({0, 2}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.out_degree({1, 2}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.out_degree({2, 2}) == static_cast<clk_lyt::degree_t>(0));

        CHECK(layout.degree({0, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 0}) == static_cast<clk_lyt::degree_t>(3));
        CHECK(layout.degree({2, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 1}) == static_cast<clk_lyt::degree_t>(4));
        CHECK(layout.degree({0, 2}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 2}) == static_cast<clk_lyt::degree_t>(3));
        CHECK(layout.degree({2, 2}) == static_cast<clk_lyt::degree_t>(2));
    }
    SECTION("USE Clocking")
    {
        clk_lyt layout{clk_lyt::aspect_ratio{2, 2, 0}, use_clocking<clk_lyt>()};

        CHECK(layout.in_degree({0, 0}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.in_degree({1, 0}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.in_degree({2, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.in_degree({1, 1}) == static_cast<clk_lyt::degree_t>(2));

        CHECK(layout.out_degree({1, 1}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.out_degree({0, 2}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.out_degree({1, 2}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.out_degree({2, 2}) == static_cast<clk_lyt::degree_t>(1));

        CHECK(layout.degree({0, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 0}) == static_cast<clk_lyt::degree_t>(3));
        CHECK(layout.degree({2, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 1}) == static_cast<clk_lyt::degree_t>(4));
        CHECK(layout.degree({0, 2}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 2}) == static_cast<clk_lyt::degree_t>(3));
        CHECK(layout.degree({2, 2}) == static_cast<clk_lyt::degree_t>(2));
    }
}
