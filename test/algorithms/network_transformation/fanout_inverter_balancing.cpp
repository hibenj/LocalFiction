//
// Created by Hien Benjamin on 27.06.2022.
//
#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/network_transformation/fanout_inverter_balancing.hpp>
#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/network_conversion.hpp>
#include <fiction/networks/technology_network.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/traits.hpp>

using namespace fiction;

TEST_CASE("Name conservation test", "[network-conversion-n]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::mig_network>>();
    maj.set_network_name("maj");

    const auto converted_maj = convert_network<mockturtle::names_view<fiction::technology_network>>(maj);

    // network name
    CHECK(converted_maj.get_network_name() == "maj");

    // PI names
    CHECK(converted_maj.get_name(converted_maj.make_signal(2)) == "a");
    CHECK(converted_maj.get_name(converted_maj.make_signal(3)) == "b");
    CHECK(converted_maj.get_name(converted_maj.make_signal(4)) == "c");

    // PO names
    CHECK(converted_maj.get_output_name(0) == "f");
}

TEST_CASE("Name conservation balance", "[network-balance]")
{
    auto maj = blueprints::fanout_inv_blc<mockturtle::names_view<mockturtle::aig_network>>();
    maj.set_network_name("fib");

    std::cout<<"NODES AIG ";
    maj.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    const auto substituted = fanout_substitution<mockturtle::names_view<fiction::technology_network>>(maj);
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

    const auto balanced_fib = mockturtle::fanout_view{inverter_balancing(fanout_substitution<mockturtle::names_view<fiction::technology_network>>(maj))};

    std::cout<<"size ";
    std::cout<<balanced_fib.size()<<std::endl;

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

    /*balanced_fib.foreach_node([&](const auto& node) {
                                  balanced_fib.foreach_fanin(node, [&](const auto& fi)
                                    {
                                        const auto new_node = balanced_fib.get_node(fi);
                                        std::cout<<"Node: "<<node<<" with fan-ins: "<<new_node<<std::endl;
                                    });
                              });
    std::cout<<std::endl;*/


    // network name
    CHECK(balanced_fib.get_network_name() == "fib");

    // PI names
    //CHECK(balanced_fib.get_name(balanced_fib.make_signal(2)) == "a");
    //CHECK(balanced_fib.get_name(balanced_fib.make_signal(3)) == "b");
    //CHECK(balanced_fib.get_name(balanced_fib.make_signal(4)) == "c");

    // PO names
    CHECK(balanced_fib.get_output_name(0) == "f");
}