//
// Created by marcel on 07.02.22.
//

#include "catch.hpp"
#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/color_routing.hpp>
#include <fiction/types.hpp>

using namespace fiction;

template <typename Spec, typename Impl>
void check_color_routing(const Spec& spec, Impl& impl, const std::vector<routing_objective<Impl>>& objectives,
                         color_routing_params ps = {})
{
    const auto success = color_routing(impl, objectives, ps);

    CHECK(success);

    check_eq(spec, impl);
}

TEST_CASE("Simple wire connection", "[color-routing]")
{
    auto spec_layout = blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>();

    // remove the wire from the implementation
    impl_layout.clear_tile({1, 1});

    // color routing should be able to recreate the removed wire
    const std::vector<routing_objective<cart_gate_clk_lyt>> objectives{{{0, 1}, {2, 1}}};

    color_routing_params ps{};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Two paths wire connections", "[color-routing]")
{
    auto spec_layout = blueprints::unbalanced_and_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::unbalanced_and_layout<cart_gate_clk_lyt>();

    // remove the wire from the implementation
    impl_layout.clear_tile({1, 2});
    impl_layout.clear_tile({2, 2});
    impl_layout.clear_tile({2, 1});

    // rip-up the AND gate's remaining connection
    impl_layout.move_node(impl_layout.get_node({2, 0}), {2, 0}, {});

    // color routing should be able to recreate the removed wire
    const std::vector<routing_objective<cart_gate_clk_lyt>> objectives{{{0, 2}, {2, 0}}, {{1, 0}, {2, 0}}};

    color_routing_params ps{};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Three paths wire connections", "[color-routing]")
{
    auto spec_layout = blueprints::three_wire_paths_gate_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::three_wire_paths_gate_layout<cart_gate_clk_lyt>();

    // remove the wire from the implementation
    impl_layout.clear_tile({1, 0});
    impl_layout.clear_tile({1, 2});
    impl_layout.clear_tile({1, 4});
    impl_layout.clear_tile({2, 0});
    impl_layout.clear_tile({2, 2});
    impl_layout.clear_tile({2, 4});
    impl_layout.clear_tile({3, 0});
    impl_layout.clear_tile({3, 2});
    impl_layout.clear_tile({3, 4});

    // color routing should be able to recreate the removed wire
    const std::vector<routing_objective<cart_gate_clk_lyt>> objectives{{{0, 0}, {4, 0}},
                                                                       {{0, 2}, {4, 2}},
                                                                       {{0, 4}, {4, 4}}};

    color_routing_params ps{};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Direct gate connections", "[color-routing]")
{
    auto spec_layout = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();

    // rip-up all connections
    impl_layout.move_node(impl_layout.get_node({2, 1}), {2, 1}, {});
    impl_layout.move_node(impl_layout.get_node({1, 0}), {1, 0}, {});
    impl_layout.move_node(impl_layout.get_node({2, 2}), {2, 2}, {});
    impl_layout.move_node(impl_layout.get_node({0, 0}), {0, 0}, {});

    // color routing should be able to recreate the removed wire
    const std::vector<routing_objective<cart_gate_clk_lyt>> objectives{
        {{1, 1}, {2, 1}}, {{2, 0}, {2, 1}}, {{3, 1}, {2, 1}}, {{1, 1}, {1, 0}},
        {{2, 0}, {1, 0}}, {{2, 1}, {2, 2}}, {{1, 0}, {0, 0}}};

    color_routing_params ps{};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}

TEST_CASE("Partial routing", "[color-routing]")
{
    auto spec_layout = blueprints::use_and_gate_layout<cart_gate_clk_lyt>();
    auto impl_layout = blueprints::use_and_gate_layout<cart_gate_clk_lyt>();

    // remove the wire from the implementation
    impl_layout.clear_tile({0, 0});
    impl_layout.clear_tile({1, 0});
    impl_layout.clear_tile({1, 1});
    impl_layout.clear_tile({2, 3});
    impl_layout.clear_tile({1, 3});
    impl_layout.clear_tile({0, 3});
    impl_layout.clear_tile({0, 2});
    impl_layout.clear_tile({2, 2});

    // rip-up all connections
    impl_layout.move_node(impl_layout.get_node({0, 1}), {0, 1}, {});
    impl_layout.move_node(impl_layout.get_node({3, 3}), {3, 3}, {});
    impl_layout.move_node(impl_layout.get_node({1, 2}), {1, 2}, {});
    impl_layout.move_node(impl_layout.get_node({3, 2}), {3, 2}, {});

    // color routing should be able to recreate the removed wire
    const std::vector<routing_objective<cart_gate_clk_lyt>> objectives{
        {{0, 1}, {1, 2}},
        {{3, 3}, {1, 2}},
        {{1, 2}, {3, 2}},
        {{0, 3}, {3, 0}}};  // additional unsatisfiable objective

    color_routing_params ps{true};

    SECTION("MCS")
    {
        ps.engine = graph_coloring_engine::MCS;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
    SECTION("SAT")
    {
        ps.engine = graph_coloring_engine::SAT;
        check_color_routing(spec_layout, impl_layout, objectives, ps);
    }
}
