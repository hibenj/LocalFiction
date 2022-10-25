//
// Created by marcel on 30.08.21.
//

#include "catch.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/traits.hpp>

#include <set>

using namespace fiction;

TEST_CASE("Traits", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    CHECK(has_synchronization_elements_v<se_layout>);
}

TEST_CASE("Clocking", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    se_layout layout{se_layout::aspect_ratio{2, 2, 0}, twoddwave_clocking<se_layout>()};

    layout.assign_synchronization_element({1, 1}, 1);

    CHECK(layout.is_clocking_scheme(clock_name::TWODDWAVE));
    CHECK(layout.is_regularly_clocked());
    CHECK(layout.num_clocks() == 4);

    CHECK(layout.get_clock_number({0, 0}) == 0);
    CHECK(layout.get_clock_number({1, 0}) == 1);
    CHECK(layout.get_clock_number({0, 1}) == 1);
    CHECK(layout.get_clock_number({1, 1}) == 2);
    CHECK(layout.get_clock_number({2, 1}) == 3);
    CHECK(layout.get_clock_number({1, 2}) == 3);
    CHECK(layout.get_clock_number({2, 2}) == 0);

    CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
    CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {2, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 2}));

    CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
    CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
    CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({2, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 2}, {1, 1}));
}

TEST_CASE("Iteration", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    se_layout layout{se_layout::aspect_ratio{2, 2, 0}, twoddwave_clocking<se_layout>()};

    layout.assign_synchronization_element({0, 1}, 1);
    layout.assign_synchronization_element({1, 0}, 1);
    layout.assign_synchronization_element({1, 2}, 1);
    layout.assign_synchronization_element({2, 1}, 1);

    CHECK(layout.incoming_clocked_zones<std::set<se_layout::coordinate>>({0, 0}).size() == 2);
    CHECK(layout.outgoing_clocked_zones<std::set<se_layout::coordinate>>({2, 2}).size() == 2);

    auto s1 = layout.incoming_clocked_zones<std::set<se_layout::coordinate>>({1, 1});
    auto s2 = std::set<se_layout::coordinate>{{{1, 0}, {0, 1}, {1, 2}, {2, 1}}};

    CHECK(s1 == s2);

    auto s3 = layout.outgoing_clocked_zones<std::set<se_layout::coordinate>>({1, 1});
    auto s4 = std::set<se_layout::coordinate>{{{1, 0}, {0, 1}, {1, 2}, {2, 1}}};

    CHECK(s3 == s4);
}

TEST_CASE("Structural properties", "[synchronization-element-layout]")
{
    using se_layout = synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    se_layout layout{se_layout::aspect_ratio{2, 2, 0}, twoddwave_clocking<se_layout>()};

    CHECK(layout.num_se() == 0);
    layout.assign_synchronization_element({0, 0}, 0);
    CHECK(layout.num_se() == 0);
    layout.assign_synchronization_element({0, 1}, 1);
    CHECK(layout.num_se() == 1);
    layout.assign_synchronization_element({1, 0}, 1);
    CHECK(layout.num_se() == 2);
    layout.assign_synchronization_element({1, 2}, 2);
    CHECK(layout.num_se() == 3);
    layout.assign_synchronization_element({2, 1}, 2);
    CHECK(layout.num_se() == 4);

    CHECK(layout.is_synchronization_element({0, 1}));
    CHECK(layout.is_synchronization_element({1, 0}));
    CHECK(layout.is_synchronization_element({1, 2}));
    CHECK(layout.is_synchronization_element({2, 1}));

    CHECK(!layout.is_synchronization_element({0, 0}));
    CHECK(!layout.is_synchronization_element({1, 1}));
    CHECK(!layout.is_synchronization_element({2, 0}));
    CHECK(!layout.is_synchronization_element({0, 2}));
    CHECK(!layout.is_synchronization_element({2, 2}));

    CHECK(layout.get_synchronization_element({0, 1}) == 1);
    CHECK(layout.get_synchronization_element({1, 0}) == 1);
    CHECK(layout.get_synchronization_element({1, 2}) == 2);
    CHECK(layout.get_synchronization_element({2, 1}) == 2);

    CHECK(layout.get_synchronization_element({0, 0}) == 0);
    CHECK(layout.get_synchronization_element({1, 1}) == 0);
    CHECK(layout.get_synchronization_element({2, 0}) == 0);
    CHECK(layout.get_synchronization_element({0, 2}) == 0);
    CHECK(layout.get_synchronization_element({2, 2}) == 0);

    CHECK(layout.in_degree({0, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.in_degree({1, 0}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.in_degree({2, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.in_degree({1, 1}) == static_cast<se_layout::degree_t>(4));

    CHECK(layout.out_degree({1, 1}) == static_cast<se_layout::degree_t>(4));
    CHECK(layout.out_degree({0, 2}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.out_degree({1, 2}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.out_degree({2, 2}) == static_cast<se_layout::degree_t>(2));

    CHECK(layout.degree({0, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 0}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.degree({2, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 1}) == static_cast<se_layout::degree_t>(4));
    CHECK(layout.degree({0, 2}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 2}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.degree({2, 2}) == static_cast<se_layout::degree_t>(2));
}
