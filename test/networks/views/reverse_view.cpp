//
// Created by Hien Benjamin on 28.06.2022.
//

#include "catch.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/networks/views/edge_color_view.hpp"
#include "fiction/networks/views/input_sort_view.hpp"
#include "fiction/networks/views/reverse_view.hpp"
#include "mockturtle/networks/aig.hpp"
#include "mockturtle/views/names_view.hpp"
#include "mockturtle/views/topo_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <mockturtle/views/fanout_view.hpp>

#include <set>

using namespace fiction;


TEST_CASE("Rev View", "[Mux-rv]")
{
    auto mux21 = blueprints::mux21_network<mockturtle::names_view<mockturtle::aig_network>>();

    mockturtle::fanout_view fov{fanout_substitution<mockturtle::names_view<technology_network>>(mux21)};
    reverse_view isv{input_sort_view{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(mux21)}}};

    std::cout<<"Input Sort foreach_node: ";
    isv.foreach_node([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

    std::cout<<"Input Sort foreach_gate: ";
    isv.foreach_gate([&](const auto& node) {std::cout<<node<<" ";});
    std::cout<<std::endl;

}