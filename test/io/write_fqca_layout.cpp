//
// Created by marcel on 23.09.21.
//

#include "catch.hpp"
#include "utils/version_info.hpp"

#include <fiction/io/write_fqca_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

#include <fmt/format.h>

#include <sstream>
#include <string>

using namespace fiction;

TEST_CASE("Write empty layout", "[fqca]")
{
    using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<coord_t>>>;

    qca_layout layout{{2, 2, 1}, "empty layout"};

    SECTION("with vias")
    {
        static const std::string fqca_layout = fmt::format("[ empty layout - generated by {} ({}) ]\n"
                                                           "\n"
                                                           "= = = \n"
                                                           "      \n"
                                                           "      \n"
                                                           "      \n"
                                                           "= = = \n"
                                                           "      \n"
                                                           "      \n"
                                                           "      \n"
                                                           "= = = \n"
                                                           "      \n"
                                                           "      \n"
                                                           "      \n"
                                                           "= = = \n"
                                                           "\n"
                                                           "\n"
                                                           "$\n"
                                                           "\n",
                                                           FICTION_VERSION, FICTION_REPO);

        std::ostringstream layout_stream{};

        write_fqca_layout(layout, layout_stream, {true});

        CHECK(layout_stream.str() == fqca_layout);
    }
    SECTION("without vias")
    {
        static const std::string fqca_layout = fmt::format("[ empty layout - generated by {} ({}) ]\n"
                                                           "\n"
                                                           "= = = \n"
                                                           "      \n"
                                                           "      \n"
                                                           "      \n"
                                                           "= = = \n"
                                                           "      \n"
                                                           "      \n"
                                                           "      \n"
                                                           "= = = \n"
                                                           "\n"
                                                           "\n"
                                                           "$\n"
                                                           "\n",
                                                           FICTION_VERSION, FICTION_REPO);

        std::ostringstream layout_stream{};

        write_fqca_layout(layout, layout_stream, {false});

        CHECK(layout_stream.str() == fqca_layout);
    }
}

TEST_CASE("Write single-layer AND gate", "[fqca]")
{
    using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<coord_t>>>;

    qca_layout layout{{4, 4}, "AND"};

    layout.assign_cell_type({0, 2}, qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 4}, qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 0}, qca_technology::cell_type::CONST_0);
    layout.assign_cell_type({2, 1}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 3}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 2}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 2}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 2}, qca_technology::cell_type::OUTPUT);

    layout.assign_cell_name({0, 2}, "a");
    layout.assign_cell_name({2, 4}, "b");
    layout.assign_cell_name({4, 2}, "f");

    static const std::string fqca_layout = fmt::format("[ AND - generated by {} ({}) ]\n"
                                                       "\n"
                                                       "= = = = = \n"
                                                       "    -     \n"
                                                       "    0     \n"
                                                       "a 0 0 0 b \n"
                                                       "    0     \n"
                                                       "    c     \n"
                                                       "= = = = = \n"
                                                       "\n"
                                                       "\n"
                                                       "$\n"
                                                       "\n"
                                                       "a:\n"
                                                       "- input\n"
                                                       "- label = \"a\"\n"
                                                       "- clock = 0\n"
                                                       "\n"
                                                       "b:\n"
                                                       "- output\n"
                                                       "- label = \"f\"\n"
                                                       "- clock = 0\n"
                                                       "\n"
                                                       "c:\n"
                                                       "- input\n"
                                                       "- label = \"b\"\n"
                                                       "- clock = 0\n"
                                                       "\n",
                                                       FICTION_VERSION, FICTION_REPO);

    std::ostringstream layout_stream{};

    write_fqca_layout(layout, layout_stream, {false});

    CHECK(layout_stream.str() == fqca_layout);
}

TEST_CASE("Exceeding cell names", "[fqca]")
{
    using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<coord_t>>>;

    qca_layout layout{{52, 0}, "[a-z]+[A-Z]+1"};

    // create layout with 53 named cells
    for (auto i = 0u; i <= 52; ++i)
    {
        layout.assign_cell_type({i, 0}, qca_technology::cell_type::NORMAL);
        layout.assign_cell_name({i, 0}, std::to_string(i));
    }

    std::ostringstream dummy_stream{};

    // since only a-z and A-Z are allowed for named cells, 53 named cells should throw an exception
    CHECK_THROWS_AS(write_fqca_layout(layout, dummy_stream), out_of_cell_names_exception);
}