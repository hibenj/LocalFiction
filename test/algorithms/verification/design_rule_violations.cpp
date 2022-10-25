//
// Created by marcel on 14.10.21.
//

#include "catch.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/verification/design_rule_violations.hpp>
#include <fiction/types.hpp>

#include <sstream>

using namespace fiction;

template <typename Lyt>
gate_level_drv_stats get_drvs(const Lyt& lyt)
{
    gate_level_drv_params ps{};
    gate_level_drv_stats  st{};

    // suppress standard output
    std::stringstream ss{};
    ps.out = &ss;

    gate_level_drvs(lyt, ps, &st);

    return st;
}

template <typename Lyt>
void check_for_drvs(const Lyt& lyt, const std::size_t num_drvs, const std::size_t num_warnings)
{
    const auto st = get_drvs(lyt);

    CHECK(st.drvs == num_drvs);
    CHECK(st.warnings == num_warnings);
}

TEST_CASE("Intact layouts", "[drv]")
{
    check_for_drvs(blueprints::and_or_gate_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::and_not_gate_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::or_not_gate_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::crossing_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::fanout_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::se_gate_layout<cart_gate_clk_lyt>(), 0, 0);
}

TEST_CASE("Warnings", "[drv]")
{
    // PI tile (1,1) is not at a border
    check_for_drvs(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(), 0, 1);
}

TEST_CASE("DRVs", "[drv]")
{
    check_for_drvs(blueprints::non_structural_all_function_gate_layout<cart_gate_clk_lyt>(), 30, 1);
}
