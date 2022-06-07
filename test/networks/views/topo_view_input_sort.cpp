//
// Created by Hien Benjamin on 30.05.2022.
//


#include "catch.hpp"
#include "mockturtle/networks/aig.hpp"
#include "mockturtle/views/topo_view.hpp"
#include "fiction/networks/views/topo_view_input_sort.hpp"

#include <set>

using namespace fiction;

TEST_CASE("Cmon", "[topo-view-input-sort]")
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

    topo_view_input_sort in_aig2{in_aig};
    nodes.clear();
    in_aig2.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 7);
    std::cout<<"InputSort: ";
    for (uint32_t i = 0; i < in_aig2.size(); ++i)
    {
        std::cout<<in_aig2.index_to_node(i)<<" ";
    }
    std::cout<<std::endl;

    mockturtle::topo_view in_aig3{in_aig};
    nodes.clear();
    in_aig3.foreach_node([&nodes](const auto& node) { nodes.insert(node); });
    CHECK(nodes.size() == 7);
    std::cout<<"Normal: ";
    for (uint32_t i = 0; i < in_aig3.size(); ++i)
    {
        std::cout<<in_aig3.index_to_node(i)<<" ";
    }
    std::cout<<std::endl;
}