//
// Created by Hien Benjamin on 30.05.2022.
//


#include "catch.hpp"


#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/technology_network.hpp"
#include "mockturtle/networks/aig.hpp"
#include "mockturtle/views/fanout_view.hpp"
#include "mockturtle/views/names_view.hpp"
#include "mockturtle/views/topo_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "fiction/networks/views/topo_view_input_sort.hpp"


using namespace fiction;

TEST_CASE("Mux_tvis", "[Mux-sort-tvis]")
{
    auto mux21 = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();
    auto tech = blueprints::maj1_network<mockturtle::names_view<technology_network>>();

    mockturtle::fanout_view fov{fanout_substitution<mockturtle::names_view<technology_network>>(mux21)};
    mockturtle::fanout_view isv{topo_view_input_sort{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(mux21)}}};

    std::cout<<"Input Sort foreach_gate: ";
    isv.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"Input Sort foreach_pi: ";
    isv.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"Mux tech ";
    tech.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    /*std::cout<<"Fanout_substitution: ";
    fov.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"Fanout_substitution Fanouts: ";
    fov.foreach_node([&](const auto& node) {if(fov.fanout_size(node)==2){std::cout<<node<<" ";}});
    std::cout<<std::endl;

    std::cout<<"Fanout_substitution input names: ";
    std::cout<<fov.get_name(fov.index_to_node(2))<<" ";
    std::cout<<fov.get_name(fov.index_to_node(3))<<" ";
    std::cout<<fov.get_name(fov.index_to_node(4))<<std::endl;

    std::cout<<"Fanout_substitution: pi: ";
    fov.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;*/
    std::cout<<"Constants: ";
    fov.foreach_node([&](const auto& node) {
                         if(fov.is_constant(node))
                             std::cout<<node<<" ";
                     });
    std::cout<<std::endl;

    std::cout<<"PIs: ";
    fov.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"FOSs ";
    fov.foreach_node([&](const auto& node) {
                         if(fov.is_fanout(node))
                             std::cout<<node<<" ";
                     });
    std::cout<<std::endl;

    std::cout<<"INVs ";
    fov.foreach_node([&](const auto& node) {
                         if(fov.is_inv(node))
                             std::cout<<node<<" ";
                     });
    std::cout<<std::endl;

    std::cout<<"ANDs ";
    fov.foreach_node([&](const auto& node) {
                         if(fov.is_and(node))
                             std::cout<<node<<" ";
                     });
    std::cout<<std::endl;



    //CHECK(isv.isFo_inv_flag()==true);

}