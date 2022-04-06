//
// Created by marcel on 05.04.22.
//

#include "catch.hpp"

#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Traits", "[sidb-bestagon-library]")
{
    CHECK(!has_post_layout_optimization_v<sidb_bestagon_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb_bestagon_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb_bestagon_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb_bestagon_library, cart_gate_clk_lyt>);
    CHECK(has_get_functional_implementations_v<sidb_bestagon_library>);
}
