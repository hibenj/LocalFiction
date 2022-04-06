//
// Created by marcel on 07.03.22.
//

#include "catch.hpp"

#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/fcn_gate_library.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/technology/sidb_surface.hpp>
#include <fiction/technology/sidb_surface_analysis.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <kitty/operations.hpp>

#include <algorithm>
#include <type_traits>
#include <vector>

using namespace fiction;

/**
 * This is simply a test library, its gates do not represent real entities that would work when fabricated.
 */
class dummy_gate_library : public fcn_gate_library<sidb_technology, 3, 3>
{
  public:
    dummy_gate_library() = delete;

    static gate_functions get_functional_implementations() noexcept
    {
        static const gate_functions implementations{{{create_id_tt(), line}, {create_and_tt(), y}}};

        return implementations;
    }

  private:
    static constexpr const fcn_gate line{cell_list_to_gate<char>({{
        {' ', 'x', ' '},
        {' ', 'x', ' '},
        {' ', 'x', ' '},
    }})};

    static constexpr const fcn_gate y{cell_list_to_gate<char>({{
        {'x', ' ', 'x'},
        {' ', 'x', ' '},
        {' ', 'x', ' '},
    }})};
};

bool contains_tt(const std::vector<kitty::dynamic_truth_table>& v, const kitty::dynamic_truth_table& elem) noexcept
{
    return std::find_if(v.cbegin(), v.cend(), [&elem](const auto& tt) { return kitty::equal(elem, tt); }) != v.cend();
}

TEST_CASE("Dummy gate library traits", "[sidb-surface-analysis]")
{
    CHECK(has_get_functional_implementations_v<dummy_gate_library>);
}

TEST_CASE("Dummy gate library simple defects", "[sidb-surface-analysis]")
{
    cart_gate_clk_lyt gate_lyt{{3, 2}};   // 4 x 3 tiles of size 3 x 3 cells each
    sidb_cell_clk_lyt cell_lyt{{11, 8}};  // makes for 12 x 9 cells

    sidb_surface defect_layout{cell_lyt};

    SECTION("defect-free")
    {
        const auto black_list = sidb_surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.empty());
    }
    SECTION("single charged defect")
    {
        defect_layout.assign_sidb_defect({6, 3}, sidb_defect_type::SI_VACANCY);

        const auto black_list = sidb_surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 5);

        REQUIRE(black_list.at({1, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 0}), create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 0}), create_and_tt()));

        REQUIRE(black_list.at({2, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 0}), create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 0}), create_and_tt()));

        REQUIRE(black_list.at({1, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 1}), create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 1}), create_and_tt()));

        REQUIRE(black_list.at({2, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 1}), create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 1}), create_and_tt()));

        REQUIRE(black_list.at({3, 1}).size() == 1);
        CHECK(contains_tt(black_list.at({3, 1}), create_and_tt()));
    }
    SECTION("single uncharged defect")
    {
        defect_layout.assign_sidb_defect({1, 1}, sidb_defect_type::SILOXANE);

        const auto black_list = sidb_surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.at({0, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({0, 0}), create_id_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_and_tt()));
    }
    SECTION("multi-defect")
    {
        defect_layout.assign_sidb_defect({6, 2}, sidb_defect_type::DB);
        defect_layout.assign_sidb_defect({6, 3}, sidb_defect_type::DB);
        defect_layout.assign_sidb_defect({3, 6}, sidb_defect_type::ETCH_PIT);
        defect_layout.assign_sidb_defect({7, 5}, sidb_defect_type::RAISED_SI);
        defect_layout.assign_sidb_defect({7, 6}, sidb_defect_type::RAISED_SI);

        const auto black_list = sidb_surface_analysis<dummy_gate_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 9);

        REQUIRE(black_list.at({1, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 0}), create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 0}), create_and_tt()));

        REQUIRE(black_list.at({2, 0}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 0}), create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 0}), create_and_tt()));

        REQUIRE(black_list.at({3, 0}).size() == 1);
        CHECK(contains_tt(black_list.at({3, 0}), create_and_tt()));

        REQUIRE(black_list.at({1, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 1}), create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 1}), create_and_tt()));

        REQUIRE(black_list.at({2, 1}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 1}), create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 1}), create_and_tt()));

        REQUIRE(black_list.at({3, 1}).size() == 1);
        CHECK(contains_tt(black_list.at({3, 1}), create_and_tt()));

        REQUIRE(black_list.at({0, 2}).size() == 1);
        CHECK(contains_tt(black_list.at({0, 2}), create_and_tt()));

        REQUIRE(black_list.at({1, 2}).size() == 2);
        CHECK(contains_tt(black_list.at({1, 2}), create_id_tt()));
        CHECK(contains_tt(black_list.at({1, 2}), create_and_tt()));

        REQUIRE(black_list.at({2, 2}).size() == 2);
        CHECK(contains_tt(black_list.at({2, 2}), create_id_tt()));
        CHECK(contains_tt(black_list.at({2, 2}), create_and_tt()));
    }
}

TEST_CASE("SiDB Bestagon gate library with simple defects", "[sidb-surface-analysis]")
{
    hex_even_col_gate_clk_lyt gate_lyt{
        aspect_ratio<hex_even_col_gate_clk_lyt>{0, 0}};  // 1 x 1 tiles of size 60 x 46 cells each
    sidb_cell_clk_lyt cell_lyt{{59, 45}};                // makes for exactly one gate of the Bestagon library

    sidb_surface defect_layout{cell_lyt};

    SECTION("defect-free")
    {
        const auto black_list = sidb_surface_analysis<sidb_bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.empty());
    }
    SECTION("single charged defect")
    {
        defect_layout.assign_sidb_defect({30, 45}, sidb_defect_type::SI_VACANCY);

        const auto black_list = sidb_surface_analysis<sidb_bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.at({0, 0}).size() == 7);
        CHECK(contains_tt(black_list.at({0, 0}), create_not_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_and_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_or_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_nand_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_nor_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_xor_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_xnor_tt()));
    }
    SECTION("single uncharged defect")
    {
        defect_layout.assign_sidb_defect({30, 45}, sidb_defect_type::SILOXANE);

        const auto black_list = sidb_surface_analysis<sidb_bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.at({0, 0}).size() == 1);
        CHECK(contains_tt(black_list.at({0, 0}), create_xnor_tt()));
    }
    SECTION("multi-defect")
    {
        defect_layout.assign_sidb_defect({16, 43}, sidb_defect_type::DB);
        defect_layout.assign_sidb_defect({30, 40}, sidb_defect_type::DB);
        defect_layout.assign_sidb_defect({41, 32}, sidb_defect_type::ETCH_PIT);
        defect_layout.assign_sidb_defect({45, 33}, sidb_defect_type::RAISED_SI);
        defect_layout.assign_sidb_defect({45, 34}, sidb_defect_type::RAISED_SI);

        const auto black_list = sidb_surface_analysis<sidb_bestagon_library>(gate_lyt, defect_layout);

        CHECK(black_list.size() == 1);

        REQUIRE(black_list.at({0, 0}).size() == 5);
        CHECK(contains_tt(black_list.at({0, 0}), create_not_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_and_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_or_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_xor_tt()));
        CHECK(contains_tt(black_list.at({0, 0}), create_xnor_tt()));
    }
}
