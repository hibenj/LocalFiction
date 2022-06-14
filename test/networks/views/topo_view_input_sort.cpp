//
// Created by Hien Benjamin on 30.05.2022.
//


#include "catch.hpp"

#include "fiction/networks/views/topo_view_input_sort.hpp"
#include "fiction/networks/views/input_sort_view.hpp"

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/technology_network.hpp"
#include "mockturtle/networks/aig.hpp"
#include "mockturtle/views/fanout_view.hpp"
#include "mockturtle/views/names_view.hpp"
#include "mockturtle/views/topo_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <set>

using namespace fiction;

TEST_CASE("Cmon", "[topo-view-input-sort]")
{
    auto mux21 = blueprints::mux21_network<mockturtle::names_view<mockturtle::aig_network>>();
    auto mux21_t = blueprints::mux21_network<mockturtle::names_view<technology_network>>();

    topo_view_input_sort isv{mux21};
    mockturtle::fanout_view fov{fanout_substitution<mockturtle::names_view<technology_network>>(input_sort_view{mux21})};
    //mockturtle::fanout_view isv_t{fiction::topo_view_input_sort{mux21_t}};

    std::cout<<"mux21: ";
    mux21.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"mux21 pi: ";
    mux21.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    /*std::cout<<"mux21_t: ";
    isv_t.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"mux21_t pi: ";
    isv_t.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"mux21_t input names: ";
    std::cout<<isv_t.get_name(isv_t.pi_at(0))<<" ";
    std::cout<<isv_t.get_name(isv_t.pi_at(1))<<" ";
    std::cout<<isv_t.get_name(isv_t.pi_at(2))<<std::endl;*/

    std::cout<<"Fanout_substitution: ";
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
    std::cout<<std::endl;

    std::cout<<"Input Sort foreach_gate: ";
    isv.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"Input Sort foreach_pi: ";
    isv.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;
}