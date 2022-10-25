//
// Created by marcel on 13.01.22.
//

#include "catch.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

// TODO empty layout

TEST_CASE("Initialize 2D gate-level bounding box", "[bounding-box]")
{
    const auto lyt_xor_maj = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();

    bounding_box_2d bb_xor_maj{lyt_xor_maj};

    CHECK(bb_xor_maj.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_xor_maj.get_max() == tile<cart_gate_clk_lyt>{3, 2});
    CHECK(bb_xor_maj.get_x_size() == 3);
    CHECK(bb_xor_maj.get_y_size() == 2);

    const auto lyt_or_not = blueprints::or_not_gate_layout<cart_gate_clk_lyt>();

    bounding_box_2d bb_or_not{lyt_or_not};

    CHECK(bb_or_not.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_or_not.get_max() == tile<cart_gate_clk_lyt>{2, 2});
    CHECK(bb_or_not.get_x_size() == 2);
    CHECK(bb_or_not.get_y_size() == 2);

    const auto lyt_crossing = blueprints::crossing_layout<cart_gate_clk_lyt>();

    bounding_box_2d bb_crossing{lyt_crossing};

    CHECK(bb_crossing.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_crossing.get_max() == tile<cart_gate_clk_lyt>{3, 2});
    CHECK(bb_crossing.get_x_size() == 3);
    CHECK(bb_crossing.get_y_size() == 2);
}

TEST_CASE("Update 2D gate-level bounding box", "[bounding-box]")
{
    auto lyt_crossing = blueprints::crossing_layout<cart_gate_clk_lyt>();

    bounding_box_2d bb_crossing{lyt_crossing};

    // resize the layout to size of 6 x 6 tiles
    lyt_crossing.resize({5, 5});

    // move the PO from tile (3, 2) to tile (2, 3) but keep its child on tile (2, 2)
    lyt_crossing.move_node(lyt_crossing.get_node({3, 2}), {2, 3},
                           {lyt_crossing.make_signal(lyt_crossing.get_node({2, 2}))});

    // still the old bounding box
    CHECK(bb_crossing.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_crossing.get_max() == tile<cart_gate_clk_lyt>{3, 2});

    bb_crossing.update_bounding_box();

    // new bounding box
    CHECK(bb_crossing.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_crossing.get_max() == tile<cart_gate_clk_lyt>{3, 3});
    CHECK(bb_crossing.get_x_size() == 3);
    CHECK(bb_crossing.get_y_size() == 3);
}

// TODO empty layout

TEST_CASE("Initialize 2D cell-level bounding box", "[bounding-box]")
{
    const auto lyt_and = blueprints::single_layer_qca_and_gate<qca_cell_clk_lyt>();

    bounding_box_2d bb_and{lyt_and};

    CHECK(bb_and.get_min() == cell<qca_cell_clk_lyt>{0, 0});
    CHECK(bb_and.get_max() == cell<qca_cell_clk_lyt>{4, 4});
    CHECK(bb_and.get_x_size() == 4);
    CHECK(bb_and.get_y_size() == 4);
}

TEST_CASE("Update 2D cell-level bounding box", "[bounding-box]")
{
    auto lyt_and = blueprints::single_layer_qca_and_gate<qca_cell_clk_lyt>();

    bounding_box_2d bb_and{lyt_and};

    // resize the layout to size of 8 x 8 cells
    lyt_and.resize({7, 7});

    // erase an input cell and the constant cell
    lyt_and.assign_cell_type({0, 2}, qca_technology::cell_type::EMPTY);
    lyt_and.assign_cell_type({2, 0}, qca_technology::cell_type::EMPTY);

    // add a wire segment below
    lyt_and.assign_cell_type({1, 6}, qca_technology::cell_type::NORMAL);
    lyt_and.assign_cell_type({2, 6}, qca_technology::cell_type::NORMAL);
    lyt_and.assign_cell_type({3, 6}, qca_technology::cell_type::NORMAL);
    lyt_and.assign_cell_type({4, 6}, qca_technology::cell_type::NORMAL);
    lyt_and.assign_cell_type({5, 6}, qca_technology::cell_type::NORMAL);

    // still the old bounding box
    CHECK(bb_and.get_min() == tile<cart_gate_clk_lyt>{0, 0});
    CHECK(bb_and.get_max() == tile<cart_gate_clk_lyt>{4, 4});

    bb_and.update_bounding_box();

    // new bounding box
    CHECK(bb_and.get_min() == tile<cart_gate_clk_lyt>{1, 1});
    CHECK(bb_and.get_max() == tile<cart_gate_clk_lyt>{5, 6});
    CHECK(bb_and.get_x_size() == 4);
    CHECK(bb_and.get_y_size() == 5);
}
