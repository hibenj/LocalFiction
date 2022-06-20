//
// Created by Hien Benjamin on 02.06.2022.
//

#include "catch.hpp"
#include "fiction/networks/views/input_sort_view.hpp"

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/networks/views/edge_color_view.hpp"
#include "fiction/networks/views/reverse_topo_view.hpp"
#include "mockturtle/networks/aig.hpp"
#include "mockturtle/views/names_view.hpp"
#include "mockturtle/views/topo_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <mockturtle/views/fanout_view.hpp>

#include <set>

using namespace fiction;

TEST_CASE("inputsorttopoview", "[input-sort-view]")
{
    auto in_aig = blueprints::input_sort_view_four<mockturtle::names_view<mockturtle::aig_network>>();


    /*TOPO VIEW*/
    input_sort_view isv{in_aig};
    //mockturtle::names_view fov{fanout_substitution<mockturtle::names_view<technology_network>>(fiction::input_sort_view{in_aig})};

    std::cout<<"Input Sort foreach_gate: ";
    isv.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"Input Sort foreach_pi: ";
    isv.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
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

    //CHECK(isv.isFo_one_inv_flag()==false);
    //CHECK(isv.isFo_two_inv_flag()==true);
}

TEST_CASE("edgecoloris", "[input-sort-view]")
{
    mockturtle::aig_network in_aig;

    const auto x1    = in_aig.create_pi();
    const auto x2    = in_aig.create_pi();
    const auto x3    = in_aig.create_pi();
    const auto gate1 = in_aig.create_and(x2, x3);
    const auto gate2 = in_aig.create_and(x1, x2);
    const auto gate3 = in_aig.create_and(gate1, gate2);

    in_aig.create_po(gate3);

    std::set<mockturtle::node<mockturtle::aig_network>> nodes;
    in_aig.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 7);

    mockturtle::topo_view in_aig2{in_aig};
    nodes.clear();
    in_aig2.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 7);

    std::cout<<"Normal private: ";
    std::for_each(nodes.begin(), nodes.end(), [&nodes](const auto& node) { std::cout<<node; });
    std::cout<<std::endl;


    fiction::input_sort_view in_aig3{in_aig};

    fiction::out_of_place_edge_color_view in_aig4{in_aig3};

    std::cout<<"Combined Sort private: ";
    in_aig4.foreach_pi([&nodes](const auto& node) {std::cout<<node;});
    std::cout<<std::endl;
}

TEST_CASE("Maj", "[Maj-sort]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();

    mockturtle::fanout_view isv{fiction::input_sort_view{maj}};

    std::cout<<"Input Sort foreach_gate: ";
    isv.foreach_node([&](const auto& node) {
                         std::cout<<node<<" ";
                     });
    std::cout<<std::endl;
    isv.foreach_node([&](const auto& node) {
                         std::cout<<isv.is_ci(node)<<" ";
                     });
    std::cout<<std::endl;
    isv.foreach_node([&](const auto& node) {
                         std::cout<<isv.is_and(node)<<" ";
                     });
    std::cout<<std::endl;

}

TEST_CASE("Mux", "[Mux-sort]")
{
    auto mux21 = blueprints::mux21_network<mockturtle::names_view<mockturtle::aig_network>>();

    mockturtle::topo_view isv{input_sort_view(mux21)};
    mockturtle::fanout_view fov{fanout_substitution<mockturtle::names_view<technology_network>>(fiction::input_sort_view{mux21})};
    //fiction::input_sort_view isv_t{mux21_t};

    /*std::cout<<"mux21: ";
    mux21.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"mux21 pi: ";
    mux21.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;*/

    std::cout<<"Input Sort foreach_gate: ";
    isv.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"Input Sort foreach_pi: ";
    isv.foreach_pi([&](const auto& node) {std::cout<<node<<" ";});
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



    CHECK(isv.isFo_one_inv_flag()==false);
    CHECK(isv.isFo_two_inv_flag()==false);

}