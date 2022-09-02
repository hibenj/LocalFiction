//
// Created by Hien Benjamin on 24.05.2022.
//

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include "fiction/algorithms/network_transformation/fanout_inverter_balancing.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/physical_design/ortho_new.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/algorithms/verification/design_rule_violations.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>


#include <type_traits>

using namespace fiction;

TEST_CASE("Zehn", "[neu]")
{
    CHECK(create_ten()==10);
}

TEST_CASE("Orthogonal mux", "[orthog]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto mux21 = blueprints::mux21_network<mockturtle::names_view<mockturtle::aig_network>>();
    mux21.set_network_name("mux21");

    const auto layout = orthogonal<gate_layout>(mux21);

    // network name
    CHECK(layout.get_layout_name() == "mux21");

    // PI names
    CHECK(layout.get_name(layout.pi_at(0)) == "c");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "a");  // third PI

    //std::cout<<layout.get_node(0);

    /*CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI*/
    /*std::cout<<"stats x"<<layout.<<std::endl;
    std::cout<<"stats y"<<<<std::endl;*/

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}



void check_new_stats(const orthogonal_physical_design_stats& st) noexcept
{
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

TEST_CASE("New Ortho mux", "[ortho-new]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto mux21 = blueprints::mux21_network<mockturtle::names_view<mockturtle::aig_network>>();
    mux21.set_network_name("mux21");

    const auto layout = orthogonal_new<gate_layout>(mux21);

    // network name
    CHECK(layout.get_layout_name() == "mux21");

    // PI names
    /*CHECK(layout.get_name(layout.pi_at(0)) == "c");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "a");  // third PI*/

    //std::cout<<layout.get_node(0);

    /*CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI*/
    /*std::cout<<"stats x"<<layout.<<std::endl;
    std::cout<<"stats y"<<<<std::endl;*/

    // PO names
    //CHECK(layout.get_output_name(0) == "f");
}

TEST_CASE("New Ortho testing", "[ortho-testing]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    //cube::coord_t

    auto mux21 = blueprints::maj1_network<mockturtle::names_view<technology_network>>();
    mux21.set_network_name("mux21");

    auto layout = orthogonal_new<gate_layout>(mux21);

    gate_level_drvs(layout);

    /*std::cout<<"Clock number on [0, 0, 0]"<<layout.get_clock_number({0, 0,0})<<std::endl;
    std::cout<<"Clock number on [1, 0, 0]"<<layout.get_clock_number({1, 0,0})<<std::endl;
    std::cout<<"Clock number on [2, 0, 0]"<<layout.get_clock_number({2, 0,0})<<std::endl;
    std::cout<<"Clock number on [3, 0, 0]"<<layout.get_clock_number({3, 0,0})<<std::endl;
    std::cout<<"Clock number on [4, 0, 0]"<<layout.get_clock_number({4, 0,0})<<std::endl;
    std::cout<<"Clock number on [5, 0, 0]"<<layout.get_clock_number({5, 0,0})<<std::endl;
    std::cout<<"Clock number on [5, 1, 0]"<<layout.get_clock_number({5, 1,0})<<std::endl;
    std::cout<<"Clock number on [6, 1, 0]"<<layout.get_clock_number({6, 1,0})<<std::endl;
    std::cout<<"Clock number on [6, 0, 0]"<<layout.get_clock_number({6, 0,0})<<std::endl;
    std::cout<<"Clock number on [7, 0, 0]"<<layout.get_clock_number({7, 0,0})<<std::endl;
    std::cout<<"Clock number on [7, 1, 0]"<<layout.get_clock_number({7, 1,0})<<std::endl;
    std::cout<<"Clock number on [8, 1, 0]"<<layout.get_clock_number({8, 1,0})<<std::endl;
    std::cout<<"Clock number on [8, 0, 0]"<<layout.get_clock_number({8, 0,0})<<std::endl;
    std::cout<<"Clock number on [9, 0, 0]"<<layout.get_clock_number({9, 0,0})<<std::endl;*/


    // network name
    CHECK(layout.get_layout_name() == "mux21");

    // PI names
    //CHECK(layout.get_name(layout.pi_at(0)) == "c");  // first PI
    //CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    //CHECK(layout.get_name(layout.pi_at(2)) == "a");  // third PI

    //std::cout<<layout.get_node(0);

    /*CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI*/
    /*std::cout<<"stats x"<<layout.<<std::endl;
    std::cout<<"stats y"<<<<std::endl;*/

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}

TEST_CASE("New Ortho maj_TEST", "[ortho-testing]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    //cube::coord_t

    auto mux21 = blueprints::TEST_maj_two_buf<mockturtle::names_view<technology_network>>();
    mux21.set_network_name("mux21");

    auto layout = orthogonal_new<gate_layout>(mux21);

    gate_level_drvs(layout);

    //print_gate_level_layout(std::cout, layout);

    /*std::cout<<"Clock number on [13, 11, 0]"<<layout.get_clock_number({13, 11, 0})<<std::endl;
    std::cout<<"Clock number on [13, 12, 0]"<<layout.get_clock_number({13, 12, 0})<<std::endl;
    std::cout<<"Clock number on [13, 13, 0]"<<layout.get_clock_number({13, 13, 0})<<std::endl;
    std::cout<<"Clock number on [13, 14, 0]"<<layout.get_clock_number({13, 14, 0})<<std::endl;
    std::cout<<"Clock number on [13, 15, 0]"<<layout.get_clock_number({13, 15, 0})<<std::endl;
    std::cout<<"Clock number on [13, 16, 0]"<<layout.get_clock_number({13, 16, 0})<<std::endl;
    std::cout<<"Clock number on [13, 17, 0]"<<layout.get_clock_number({13, 17, 0})<<std::endl;
    std::cout<<"Clock number on [13, 18, 0]"<<layout.get_clock_number({13, 18, 0})<<std::endl;
    std::cout<<"Clock number on [13, 19, 0]"<<layout.get_clock_number({13, 19, 0})<<std::endl;
    std::cout<<"Clock number on [14, 19, 0]"<<layout.get_clock_number({14, 19,0})<<std::endl;
    std::cout<<"Clock number on [14, 20, 0]"<<layout.get_clock_number({14, 20,0})<<std::endl;
    std::cout<<"Clock number on [13, 20, 0]"<<layout.get_clock_number({13, 20,0})<<std::endl;
    std::cout<<"Clock number on [13, 21, 0]"<<layout.get_clock_number({13, 21,0})<<std::endl;
    std::cout<<"Clock number on [14, 21, 0]"<<layout.get_clock_number({14, 21,0})<<std::endl;
    std::cout<<"Clock number on [14, 22, 0]"<<layout.get_clock_number({14, 22,0})<<std::endl;
    std::cout<<"Clock number on [13, 22, 0]"<<layout.get_clock_number({13, 22,0})<<std::endl;
    std::cout<<"Clock number on [13, 23, 0]"<<layout.get_clock_number({13, 23,0})<<std::endl;
    std::cout<<"Clock number on [14, 23, 0]"<<layout.get_clock_number({14, 23,0})<<std::endl;
    std::cout<<"Clock number on [15, 23, 0]"<<layout.get_clock_number({15, 23,0})<<std::endl;*/

    /*std::cout<<"Clock number on [12, 17, 0]"<<layout.get_clock_number({12, 17,0})<<std::endl;
    std::cout<<"Clock number on [12, 18, 0]"<<layout.get_clock_number({12, 18,0})<<std::endl;
    std::cout<<"Clock number on [12, 19, 0]"<<layout.get_clock_number({12, 19,0})<<std::endl;
    std::cout<<"Clock number on [12, 20, 0]"<<layout.get_clock_number({12, 20,0})<<std::endl;
    std::cout<<"Clock number on [12, 21, 0]"<<layout.get_clock_number({12, 21,0})<<std::endl;
    std::cout<<"Clock number on [12, 22, 0]"<<layout.get_clock_number({12, 22,0})<<std::endl;
    std::cout<<"Clock number on [12, 23, 0]"<<layout.get_clock_number({12, 23,0})<<std::endl;*/




    // network name
    CHECK(layout.get_layout_name() == "mux21");

    // PI names
    //CHECK(layout.get_name(layout.pi_at(0)) == "c");  // first PI
    //CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    //CHECK(layout.get_name(layout.pi_at(2)) == "a");  // third PI

    //std::cout<<layout.get_node(0);

    /*CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI*/
    /*std::cout<<"stats x"<<layout.<<std::endl;
    std::cout<<"stats y"<<<<std::endl;*/

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}
