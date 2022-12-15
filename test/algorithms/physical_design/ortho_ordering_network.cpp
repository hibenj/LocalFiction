//
// Created by Hien Benjamin on 15.12.2022.
//
#include "catch.hpp"
#include "fiction/networks/sequential_technology_network.hpp"
#include <fiction/networks/technology_network.hpp>
//#include <fiction/algorithms/physical_design/ortho_new.hpp>
//#include <fiction/algorithms/physical_design/ortho_ordering_network.hpp>

//using namespace fiction;

/*
TEST_CASE("Ordering_Network_mux21", "[ortho_ordering]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto mux21 = blueprints::mux21_network<mockturtle::names_view<mockturtle::aig_network>>();

    const auto layout = orthogonal_ordering_network<gate_layout>(mux21);

    fiction::debug::write_dot_layout(layout, "Ordering_Network_mux21");
}
*/
