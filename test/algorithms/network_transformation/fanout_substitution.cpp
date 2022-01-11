//
// Created by marcel on 25.05.21.
//

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/networks/topology_network.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>

#include <type_traits>

using namespace fiction;

template <typename Ntk>
void substitute(const Ntk& ntk, const fanout_substitution_params ps, const uint32_t size)
{
    const auto substituted = fanout_substitution<topology_network>(ntk, ps);

    CHECK(substituted.size() == size);
    CHECK(is_fanout_substituted(substituted, ps));

    check_eq(ntk, substituted);
}

TEST_CASE("Name conservation", "[fanout-substitution]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::mig_network>>();
    maj.set_network_name("maj");

    const auto substituted_maj = fanout_substitution<mockturtle::names_view<fiction::topology_network>>(maj);

    // network name
    CHECK(substituted_maj.get_network_name() == "maj");

    // PI names
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(2)) == "a");
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(3)) == "b");
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(4)) == "c");

    // PO names
    CHECK(substituted_maj.get_output_name(0) == "f");
}

TEST_CASE("Simple fanout substitution", "[fanout-substitution]")
{
    const auto top = blueprints::multi_output_and_network<topology_network>();

    fanout_substitution_params ps_depth{fanout_substitution_params::substitution_strategy::DEPTH};
    fanout_substitution_params ps_breadth{fanout_substitution_params::substitution_strategy::BREADTH};

    substitute(top, ps_depth, top.size() + 3);
    substitute(top, ps_breadth, top.size() + 3);
}

TEST_CASE("Complex fanout substitution", "[fanout-substitution]")
{
    const auto top = blueprints::maj4_network<topology_network>();
    CHECK(!is_fanout_substituted(top));

    fanout_substitution_params ps_depth{fanout_substitution_params::substitution_strategy::DEPTH};
    fanout_substitution_params ps_breadth{fanout_substitution_params::substitution_strategy::BREADTH};

    substitute(top, ps_depth, top.size() + 7);

    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();
    CHECK(!is_fanout_substituted(aig));
    substitute(aig, ps_depth, aig.size() + 41);
    substitute(aig, ps_breadth, aig.size() + 41);
}

TEST_CASE("Degree and threshold in fanout substitution", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    fanout_substitution_params ps_31{fanout_substitution_params::substitution_strategy::BREADTH, 3, 1};
    fanout_substitution_params ps_22{fanout_substitution_params::substitution_strategy::DEPTH, 2, 2};

    substitute(aig, ps_31, aig.size() + 35);
    substitute(aig, ps_22, aig.size() + 34);
}

TEST_CASE("Consistent network size after multiple fanout substitutions", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    auto substituted = fanout_substitution<topology_network>(aig);

    auto subsubsubsubstituted = fanout_substitution<topology_network>(
        fanout_substitution<topology_network>(fanout_substitution<topology_network>(substituted)));

    CHECK(substituted.size() == subsubsubsubstituted.size());
}

TEST_CASE("Consistent fanout substitution after balancing", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    auto substituted = fanout_substitution<topology_network>(aig);

    CHECK(is_fanout_substituted(substituted));
    auto balanced = network_balancing<topology_network>(substituted);
    CHECK(is_fanout_substituted(balanced));

    auto top = blueprints::fanout_substitution_corner_case_network<topology_network>();

    auto substituted_top = fanout_substitution<topology_network>(top);
    CHECK(is_fanout_substituted(substituted_top));
    auto balanced_top = network_balancing<topology_network>(substituted_top);
    CHECK(is_fanout_substituted(balanced_top));
}