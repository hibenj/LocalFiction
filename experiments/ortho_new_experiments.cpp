//
// Created by Hien Benjamin on 12.09.2022.
//
#include "fiction_experiments.hpp"
#include "fiction/networks/sequential_technology_network.hpp"
#include "fiction/algorithms/physical_design/ortho_new.hpp"
//#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "../test/utils/blueprints/network_blueprints.hpp"
#include "fiction/algorithms/physical_design/apply_gate_library.hpp"
#include "fiction/io/write_svg_layout.hpp"
#include "fiction/technology/qca_one_library.hpp"
#include "fiction/utils/debug/network_writer.hpp"
#include "mockturtle/io/bench_reader.hpp"
#include "mockturtle/io/blif_reader.hpp"
#include "mockturtle/io/verilog_reader.hpp"

#include <iostream>

using namespace fiction;

std::string benchmark_path_blif(const std::string& benchmark_name, const std::string& benchmark_folder = "../benchmarks")
{
    return fmt::format("{}{}/{}.blif", EXPERIMENTS_PATH, benchmark_folder, benchmark_name);
}
std::string benchmark_path_bench(const std::string& benchmark_name, const std::string& benchmark_folder = "../benchmarks")
{
    return fmt::format("{}{}/{}.bench", EXPERIMENTS_PATH, benchmark_folder, benchmark_name);
}


int main()
{
    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, double, double, uint64_t, uint64_t, uint64_t, uint64_t>
        ortho_new_exp( "Ortho_majority_placement",
                      "Benchmark",
                      "State of the art size",
                      "ortho_new size",
                      "size diff",
                      "State of the art runtime",
                      "ortho_new runtime",
                      "State of the art gate number",
                      "ortho_new gate number",
                      "State of the art wire number",
                      "ortho_new wire number");

    fiction::orthogonal_physical_design_stats st_aig;
    fiction::orthogonal_physical_design_stats st_tech;

    /**for custom circuits**/
    /*static constexpr const std::array benchmark_names {"mux21", "maj1", "maj4", "one_and", "two_and", "three_and", "four_and", "maj_to_maj"};

    auto mux21 = blueprints::mux21_network<mockturtle::names_view<mockturtle::aig_network>>();
    auto mux21_t = blueprints::mux21_network<mockturtle::names_view<fiction::technology_network>>();

    auto maj1 = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();
    auto maj1_t = blueprints::maj1_network<mockturtle::names_view<fiction::technology_network>>();

    auto maj4 = blueprints::maj4_network<mockturtle::names_view<mockturtle::aig_network>>();
    auto maj4_t = blueprints::maj4_network<mockturtle::names_view<fiction::technology_network>>();

    auto one_and = blueprints::TEST_maj_one_buf<mockturtle::names_view<mockturtle::aig_network>>();
    auto one_and_t = blueprints::TEST_maj_one_buf<mockturtle::names_view<fiction::technology_network>>();

    auto two_and = blueprints::TEST_maj_two_buf<mockturtle::names_view<mockturtle::aig_network>>();
    auto two_and_t = blueprints::TEST_maj_two_buf<mockturtle::names_view<fiction::technology_network>>();

    auto three_and = blueprints::TEST_maj_three_buf<mockturtle::names_view<mockturtle::aig_network>>();
    auto three_and_t = blueprints::TEST_maj_three_buf<mockturtle::names_view<fiction::technology_network>>();

    auto four_and = blueprints::TEST_maj_four_buf<mockturtle::names_view<mockturtle::aig_network>>();
    auto four_and_t = blueprints::TEST_maj_four_buf<mockturtle::names_view<fiction::technology_network>>();

    auto maj_to_maj = blueprints::TEST_maj_maj_buf<mockturtle::names_view<mockturtle::aig_network>>();
    auto maj_to_maj_t = blueprints::TEST_maj_maj_buf<mockturtle::names_view<fiction::technology_network>>();

    std::vector<mockturtle::names_view<mockturtle::aig_network>> bms_aig = {mux21, maj1, maj4, one_and, two_and, three_and, four_and, maj_to_maj};
    std::vector<mockturtle::names_view<fiction::technology_network>> bms_tech = {mux21_t, maj1_t, maj4_t, one_and_t, two_and_t, three_and_t, four_and_t, maj_to_maj_t};

    for (int i = 0; i < bms_aig.size(); ++i)
    {
        auto bm_aig = bms_aig[i];

        auto bm_tech = bms_tech[i];

        auto bm_name = benchmark_names[i];

        fmt::print( "[i] processing {}\n", bm_name );

        std::cout<<"Size "<<bms_aig.size()<<std::endl;



        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        const auto layout_one = fiction::orthogonal_new<gate_layout>(bm_aig, {}, &st_aig);

        const auto layout_two = fiction::orthogonal_new<gate_layout>(bm_tech, {}, &st_tech);

        ortho_new_exp(bm_name, (st_aig.x_size-1) * (st_aig.y_size-1), (st_tech.x_size-1) * (st_tech.y_size-1), (st_aig.x_size-1) * (st_aig.y_size-1) - (st_tech.x_size-1) * (st_tech.y_size-1), mockturtle::to_seconds(st_aig.time_total),mockturtle::to_seconds(st_tech.time_total),
                      st_aig.num_gates, st_tech.num_gates, st_aig.num_wires, st_tech.num_wires);
    }*/

    /**for sequential circuits**/
    /*fiction::technology_network tech_two;
    mockturtle::klut_network klut;
    mockturtle::aig_network aig_bench;

    const std::string& benchmark = "test_bench/b04";

    const auto read_blif_result =
        lorina::read_bench( benchmark_path_bench(benchmark), mockturtle::bench_reader( klut ) );
    assert(read_blif_result == lorina::return_code::success);
    klut.foreach_node(
        [&](const auto& n)
        {
            std::cout<<"ONode "<<n<<std::endl;
        });

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    const auto layout_two = fiction::orthogonal_new<gate_layout>(klut, {}, &st_tech);*/

    /**for combinational benchmarks**/
    static constexpr const uint64_t bench_select = fiction_experiments::trindade16;

    const std::string benchmark = "fontes18/2bitAdderMaj";

    /*for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {*/
        fmt::print( "[i] processing {}\n", benchmark );

        mockturtle::aig_network aig;
        fiction::technology_network tech;

        mockturtle::klut_network klut;



        const auto read_verilog_result_aig =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(aig));
        assert(read_verilog_result_aig == lorina::return_code::success);

        const auto read_verilog_result_tech =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(tech));
        assert(read_verilog_result_tech == lorina::return_code::success);

        tech.foreach_node(
            [&](const auto& n)
            {
                std::cout<<"Node "<<n<<std::endl;
            });

        fiction::debug::write_dot_network(aig, "one");


        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        const auto layout_one = fiction::orthogonal<gate_layout>(aig, {}, &st_aig);

        //fiction::debug::write_dot_layout(layout_two);

        const auto cell_level_lyt_o = apply_gate_library<qca_cell_clk_lyt, qca_one_library>(layout_one);

        write_qca_layout_svg(cell_level_lyt_o, "2bitAdderMaj_ortho.svg");

        const auto layout_two = fiction::orthogonal_new<gate_layout>(aig, {}, &st_tech);

        /*const auto cell_level_lyt = apply_gate_library<qca_cell_clk_lyt, qca_one_library>(layout_two);

        write_qca_layout_svg(cell_level_lyt, "2bitAdderMaj_ortho_new.svg");*/

        fiction::debug::write_dot_layout(layout_two);

        ortho_new_exp(benchmark, (st_aig.x_size-1) * (st_aig.y_size-1), (st_tech.x_size-1) * (st_tech.y_size-1), (st_aig.x_size-1) * (st_aig.y_size-1) - (st_tech.x_size-1) * (st_tech.y_size-1), mockturtle::to_seconds(st_aig.time_total),mockturtle::to_seconds(st_tech.time_total),
                      st_aig.num_gates, st_tech.num_gates, st_aig.num_wires, st_tech.num_wires);
    //}
    ortho_new_exp.save();
    ortho_new_exp.table();

}