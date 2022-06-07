//
// Created by Hien Benjamin on 02.06.2022.
//
#include "catch.hpp"

#include "fiction/networks/views/input_sort_view.hpp"
#include "fiction/networks/views/edge_color_view.hpp"
#include "mockturtle/networks/aig.hpp"
#include "mockturtle/views/names_view.hpp"
#include "mockturtle/views/topo_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include <mockturtle/views/fanout_view.hpp>


#include <set>

using namespace fiction;

TEST_CASE("inputsorttopoview", "[input-sort-view]")
{
    mockturtle::aig_network in_aig;

    const auto x1    = in_aig.create_pi();
    const auto x2    = in_aig.create_pi();
    const auto x3    = in_aig.create_pi();
    const auto gate1 = in_aig.create_and(x2, x3);
    const auto gate2 = in_aig.create_and(x1, x2);
    const auto gate3 = in_aig.create_and(gate1, gate2);

    in_aig.create_po(gate3);

    /*PLAIN*/
    std::set<mockturtle::node<mockturtle::aig_network>> nodes;
    in_aig.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 7);

    /*TOPO VIEW*/
    mockturtle::topo_view in_aig2{in_aig};
    nodes.clear();
    in_aig2.foreach_node([&](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 7);

    std::cout<<"Topo View private: ";
    std::for_each(nodes.begin(), nodes.end(), [&nodes](const auto& node) { std::cout<<node; });
    std::cout<<std::endl;

    /*INPUT SORT VIEW*/
    fiction::input_sort_view in_aig3{in_aig};
    nodes.clear();
    in_aig3.foreach_node([&nodes](const auto& node) { nodes.insert(node);});
    //std::for_each(nodes.begin(), nodes.end(), [&nodes](const auto& node) { std::cout<<node; });
    CHECK(nodes.size() == 7);

    std::cout<<"Input Sort foreach_gate: ";
    in_aig3.foreach_node([&nodes](const auto& node) {std::cout<<node;});
    std::cout<<std::endl;

    /*INPUT SORT VIEW*/
    auto store = in_aig3.getTopo_order_input_sort();
    std::cout<<"Input Sort private: ";
    std::for_each(store.begin(), store.end(), [&store](const auto& node) { std::cout<<node; });
    std::cout<<std::endl;
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

/*    mockturtle::aig_network in_aig;

    const auto x1    = in_aig.create_pi();
    const auto x2    = in_aig.create_pi();
    const auto x3    = in_aig.create_pi();
    const auto gate1 = in_aig.create_and(x2, x3);
    const auto gate2 = in_aig.create_and(x1, x2);
    const auto gate3 = in_aig.create_and(gate1, gate2);

    in_aig.create_po(gate3);*/

    fiction::input_sort_view isv{maj};

    std::cout<<"Input Sort foreach_gate: ";
    isv.foreach_node([&](const auto& node) {std::cout<<node;});
    std::cout<<std::endl;

}

TEST_CASE("Mux", "[Mux-sort]")
{
    auto mux21 = blueprints::mux21_network<mockturtle::names_view<mockturtle::aig_network>>();

    mockturtle::fanout_view isv{fiction::input_sort_view(mux21)};

    std::cout<<"Input Sort foreach_gate: ";
    isv.foreach_node([&](const auto& node) {std::cout<<node;});
    std::cout<<std::endl;

    std::cout<<"Input Sort foreach_pi: ";
    isv.foreach_pi([&](const auto& node) {std::cout<<node;});
    std::cout<<std::endl;

}