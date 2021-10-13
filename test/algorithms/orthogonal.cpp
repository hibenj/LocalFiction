//
// Created by marcel on 02.06.21.
//

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking.hpp"

#include <fiction/algorithms/apply_gate_library.hpp>
#include <fiction/algorithms/orthogonal.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/topology_network.hpp>
#include <fiction/technology/qca_one_library.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <type_traits>

using namespace fiction;

TEST_CASE("Number of constant fanins", "[orthogonal]")
{
    const auto maj4 = blueprints::maj4_network<mockturtle::mig_network>();

    maj4.foreach_node([&maj4](const auto& n) { CHECK(detail::num_constant_fanins(maj4, n) == 0ul); });

    const auto and_inv = blueprints::unbalanced_and_inv_network<mockturtle::mig_network>();

    CHECK(detail::num_constant_fanins(and_inv, 3) == 1ul);
}

TEST_CASE("High-degree fanin nodes", "[orthogonal]")
{
    const auto maj4 = blueprints::maj4_network<mockturtle::mig_network>();

    CHECK(detail::has_high_degree_fanin_nodes(maj4, 2));
    CHECK(!detail::has_high_degree_fanin_nodes(maj4, 3));

    const auto and_inv = blueprints::unbalanced_and_inv_network<mockturtle::mig_network>();

    CHECK(detail::has_high_degree_fanin_nodes(and_inv, 1));
    CHECK(!detail::has_high_degree_fanin_nodes(and_inv, 2));
    CHECK(!detail::has_high_degree_fanin_nodes(and_inv, 3));
}

TEST_CASE("East-south coloring", "[orthogonal]")
{
    const auto check = [](const auto& ntk)
    {
        auto container = detail::east_south_coloring(ntk);
        CHECK(detail::is_east_south_colored(container.color_ntk));
    };

    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::maj1_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::maj4_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::se_coloring_corner_case_network<topology_network>())});
    check(mockturtle::fanout_view{fanout_substitution<topology_network>(
        blueprints::fanout_substitution_corner_case_network<topology_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::nary_operation_network<topology_network>())});
    check(mockturtle::fanout_view{fanout_substitution<topology_network>(blueprints::clpl<topology_network>())});
}


void check_stats(const orthogonal_physical_design_stats& st) noexcept
{
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

TEST_CASE("Layout equivalence", "[orthogonal]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coord_t>>>>;

    const auto check = [](const auto& ntk)
    {
        orthogonal_physical_design_stats stats{};

        auto layout = orthogonal<gate_layout>(ntk, {}, &stats);

        print_gate_level_layout(std::cout, layout);

        check_stats(stats);
        check_eq(ntk, layout);
    };

    check(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check(blueprints::maj1_network<mockturtle::aig_network>());
    check(blueprints::maj4_network<mockturtle::aig_network>());
    check(blueprints::se_coloring_corner_case_network<topology_network>());
    check(blueprints::fanout_substitution_corner_case_network<topology_network>());
    check(blueprints::nary_operation_network<topology_network>());
    check(blueprints::clpl<topology_network>());

    // constant input network
    check(blueprints::unbalanced_and_inv_network<mockturtle::mig_network>());
}

TEST_CASE("Gate library application", "[orthogonal]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coord_t>>>>;
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<coord_t>>>;

    const auto check = [](const auto& ntk)
    {
        orthogonal_physical_design_stats stats{};

        auto layout = orthogonal<gate_layout>(ntk, {}, &stats);

        CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(layout));
    };

    check(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check(blueprints::maj1_network<mockturtle::aig_network>());
    check(blueprints::maj4_network<mockturtle::aig_network>());
    check(blueprints::se_coloring_corner_case_network<topology_network>());
    check(blueprints::fanout_substitution_corner_case_network<topology_network>());
    check(blueprints::clpl<topology_network>());

    // constant input network
    check(blueprints::unbalanced_and_inv_network<mockturtle::mig_network>());
}

TEST_CASE("Name conservation", "[algorithms]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coord_t>>>>;

    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();
    maj.set_network_name("maj");

    const auto layout = orthogonal<gate_layout>(maj);

    // network name
    CHECK(layout.get_layout_name() == "maj");

    // PI names
    CHECK(layout.get_name(2) == "a");  // first PI
    CHECK(layout.get_name(3) == "b");  // second PI
    CHECK(layout.get_name(4) == "c");  // third PI

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}
