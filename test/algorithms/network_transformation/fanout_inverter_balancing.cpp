/*
//
// Created by Hien Benjamin on 27.06.2022.
//
#include "catch.hpp"
#include "mockturtle/views/fanout_view.hpp"
#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/network_transformation/fanout_inverter_balancing.hpp>
#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/network_conversion.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/traits.hpp>

using namespace fiction;

TEST_CASE("Name conservation balance", "[network-balance]")
{
    auto fib = blueprints::fanout_inv_blc<mockturtle::names_view<mockturtle::aig_network>>();
    fib.set_network_name("fib");

    std::cout<<"NODES AIG ";
    fib.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    const auto substituted = mockturtle::fanout_view(fanout_substitution<mockturtle::names_view<technology_network>>(fib));

    std::cout<<"NODES FOS ";
    substituted.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"PIs: ";
    substituted.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"FOSs ";
    substituted.foreach_node([&](const auto& node) {
                         if(substituted.is_fanout(node))
                             std::cout<<node<<" ";
                     });
    std::cout<<std::endl;

    std::cout<<"INVs ";
    substituted.foreach_node([&](const auto& node) {
                         if(substituted.is_inv(node))
                             std::cout<<node<<" ";
                     });
    std::cout<<std::endl;

    std::cout<<"ANDs ";
    substituted.foreach_node([&](const auto& node) {
                         if(substituted.is_and(node))
                             std::cout<<node<<" ";
                     });
    std::cout<<std::endl;

    const auto balanced_fib = inverter_balancing<mockturtle::fanout_view<mockturtle::names_view<technology_network>>>(substituted);

    std::cout<<"NODES BLC ";
    balanced_fib.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"PIs: ";
    balanced_fib.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"FOSs ";
    balanced_fib.foreach_node([&](const auto& node) {
                                 if(balanced_fib.is_fanout(node))
                                     std::cout<<node<<" ";
                             });
    std::cout<<std::endl;

    std::cout<<"INVs ";
    balanced_fib.foreach_node([&](const auto& node) {
                                 if(balanced_fib.is_inv(node))
                                     std::cout<<node<<" ";
                             });
    std::cout<<std::endl;

    std::cout<<"ANDs ";
    balanced_fib.foreach_node([&](const auto& node) {
                                 if(balanced_fib.is_and(node))
                                     std::cout<<node<<" ";
                             });
    std::cout<<std::endl;

    // network name
    CHECK(balanced_fib.get_network_name() == "fib");

    // PI names
    //CHECK(balanced_fib.get_name(balanced_fib.make_signal(2)) == "a");
    //CHECK(balanced_fib.get_name(balanced_fib.make_signal(3)) == "b");
    //CHECK(balanced_fib.get_name(balanced_fib.make_signal(4)) == "c");

    // PO names
    CHECK(balanced_fib.get_output_name(0) == "f");
}*/
