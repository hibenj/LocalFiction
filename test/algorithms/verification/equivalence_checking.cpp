//
// Created by marcel on 14.10.21.
//

#include "catch.hpp"
#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/verification/equivalence_checking.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>

using namespace fiction;

template <typename Spec, typename Impl>
void check_for_strong_equiv(const Spec& spec, const Impl& impl)
{
    equivalence_checking_stats st{};

    equivalence_checking(spec, impl, &st);

    CHECK(st.eq == eq_type::STRONG);
    CHECK(st.counter_example.empty());
    CHECK(st.spec_drv_stats.drvs == 0);
    CHECK(st.impl_drv_stats.drvs == 0);
    CHECK(st.tp_diff == 0);
}

template <typename Spec, typename Impl>
void check_for_weak_equiv(const Spec& spec, const Impl& impl)
{
    equivalence_checking_stats st{};

    equivalence_checking(spec, impl, &st);

    CHECK(st.eq == eq_type::WEAK);
    CHECK(st.counter_example.empty());
    CHECK(st.spec_drv_stats.drvs == 0);
    CHECK(st.impl_drv_stats.drvs == 0);
    CHECK(st.tp_diff > 0);
}

template <typename Spec, typename Impl>
void check_for_no_equiv(const Spec& spec, const Impl& impl)
{
    equivalence_checking_stats st{};

    equivalence_checking(spec, impl, &st);

    CHECK(st.eq == eq_type::NO);
    CHECK(((!st.counter_example.empty()) || (st.spec_drv_stats.drvs != 0) || (st.impl_drv_stats.drvs != 0)));
}

TEST_CASE("Network-network equivalence", "[equiv]")
{
    check_for_strong_equiv(blueprints::maj4_network<mockturtle::aig_network>(),
                           blueprints::maj4_network<mockturtle::mig_network>());
    check_for_strong_equiv(blueprints::maj4_network<mockturtle::mig_network>(),
                           blueprints::maj4_network<mockturtle::xag_network>());
    check_for_strong_equiv(blueprints::maj4_network<mockturtle::xag_network>(),
                           blueprints::maj4_network<fiction::technology_network>());
    check_for_strong_equiv(blueprints::maj4_network<fiction::technology_network>(),
                           blueprints::maj4_network<mockturtle::aig_network>());
}

TEST_CASE("Network-layout equivalence", "[equiv]")
{
    SECTION("Cartesian layout")
    {
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::aig_network>(),
                               blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::mig_network>(),
                               blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::xag_network>(),
                               blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<fiction::technology_network>(),
                               blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
    }
    SECTION("Hexagonal layout")
    {
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::aig_network>(),
                               blueprints::and_or_gate_layout<hex_even_col_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::mig_network>(),
                               blueprints::and_or_gate_layout<hex_odd_col_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::xag_network>(),
                               blueprints::and_or_gate_layout<hex_even_row_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<fiction::technology_network>(),
                               blueprints::and_or_gate_layout<hex_odd_row_gate_clk_lyt>());
    }
}

TEST_CASE("Layout-layout equivalence", "[equiv]")
{
    SECTION("TP == 1/1")
    {
        check_for_strong_equiv(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(),
                               blueprints::xor_maj_gate_layout<hex_even_col_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(),
                               blueprints::xor_maj_gate_layout<hex_odd_col_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(),
                               blueprints::xor_maj_gate_layout<hex_even_row_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(),
                               blueprints::xor_maj_gate_layout<hex_odd_row_gate_clk_lyt>());
    }
    SECTION("TP == 1/2")
    {
        check_for_strong_equiv(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(),
                               blueprints::unbalanced_and_layout<hex_even_col_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(),
                               blueprints::unbalanced_and_layout<hex_odd_col_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(),
                               blueprints::unbalanced_and_layout<hex_even_row_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(),
                               blueprints::unbalanced_and_layout<hex_odd_row_gate_clk_lyt>());
    }
}

TEST_CASE("Weak equivalence", "[equiv]")
{
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<mockturtle::aig_network>(),
                         blueprints::unbalanced_and_layout<cart_gate_clk_lyt>());
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<mockturtle::mig_network>(),
                         blueprints::unbalanced_and_layout<hex_even_col_gate_clk_lyt>());
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<mockturtle::xag_network>(),
                         blueprints::unbalanced_and_layout<hex_odd_col_gate_clk_lyt>());
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<mockturtle::klut_network>(),
                         blueprints::unbalanced_and_layout<hex_even_row_gate_clk_lyt>());
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<fiction::technology_network>(),
                         blueprints::unbalanced_and_layout<hex_odd_row_gate_clk_lyt>());
}

TEST_CASE("No equivalence", "[equiv]")
{
    check_for_no_equiv(blueprints::and_or_network<mockturtle::xag_network>(),
                       blueprints::half_adder_network<fiction::technology_network>());
    check_for_no_equiv(blueprints::full_adder_network<mockturtle::aig_network>(),
                       blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>());
    check_for_no_equiv(blueprints::half_adder_network<mockturtle::mig_network>(),
                       blueprints::and_or_gate_layout<hex_even_row_gate_clk_lyt>());
    check_for_no_equiv(blueprints::and_not_gate_layout<hex_odd_row_gate_clk_lyt>(),
                       blueprints::and_or_gate_layout<hex_even_col_gate_clk_lyt>());
}
