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
#include "fiction/technology/technology_mapping_library.hpp"
#include "fiction/utils/debug/network_writer.hpp"
#include "mockturtle/algorithms/mapper.hpp"
#include "mockturtle/io/bench_reader.hpp"
#include "mockturtle/io/blif_reader.hpp"
#include "mockturtle/io/genlib_reader.hpp"
#include "mockturtle/io/verilog_reader.hpp"

#include <fiction/algorithms/physical_design/ortho_majority_gates_network.hpp>
#include <fiction/algorithms/physical_design/ortho_ordering_network.hpp>
#include <fiction/algorithms/physical_design/ortho_sequential_network.hpp>

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
    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, double, double, uint64_t, uint64_t, uint64_t, uint64_t>
        ortho_new_exp( "Ortho_majority_placement",
                      "Benchmark", "w", "h",
                      "State of the art size",
                      "ortho_new size",
                      "size diff",
                      "State of the art runtime",
                      "ortho_new runtime",
                      "State of the art gate number",
                      "ortho_new gate number",
                      "State of the art wire number",
                      "ortho_new wire number");

    fiction::orthogonal_physical_design_stats st_aig_one;
    fiction::orthogonal_physical_design_stats st_aig_two;
    fiction::orthogonal_physical_design_stats st_tech_one;
    fiction::orthogonal_physical_design_stats st_tech_two;

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
    // instantiate a technology mapping library
    std::stringstream library_stream{};
    library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV
                   << fiction::GATE_AND2 << fiction::GATE_OR2;

    std::vector<mockturtle::gate> gates{};

    // parameters for technology mapping
    mockturtle::map_params map_params{};

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
    assert(read_genlib_result == lorina::return_code::success);
    mockturtle::tech_library<2> gate_lib{gates};

    fiction::technology_network tech_two;
    mockturtle::klut_network klut;
    mockturtle::aig_network aig_bench;

    const std::string& benchmark = "test_bench/b01";

    const auto read_blif_result =
        lorina::read_bench( benchmark_path_bench(benchmark), mockturtle::bench_reader( klut ) );
    assert(read_blif_result == lorina::return_code::success);
    klut.foreach_node(
        [&](const auto& n)
        {
            std::cout<<"ONode "<<n<<std::endl;
        });

    // perform technology mapping
    const auto mapped_network = mockturtle::map(klut, gate_lib, map_params);

    mapped_network.foreach_node(
        [&](const auto& n)
        {
            std::cout<<"MNode "<<n<<std::endl;
        });

    fiction::debug::write_dot_network(klut, fmt::format("{}{}", benchmark, "logic_aig_sequential"));

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    //const auto layout_two = fiction::orthogonal<gate_layout>(klut, {}, &st_tech_one);

    /**for combinational benchmarks**/

    // instantiate a technology mapping library
    /*std::stringstream library_stream{};
    library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF << fiction::GATE_INV
                   << fiction::GATE_AND2 << fiction::GATE_OR2;

    std::vector<mockturtle::gate> gates{};

    // parameters for technology mapping
    mockturtle::map_params map_params{};

    const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
    assert(read_genlib_result == lorina::return_code::success);
    mockturtle::tech_library<2> gate_lib{gates};

    *//*static constexpr const std::array ISCAS {
        "ISCAS85/c17", "ISCAS85/c432", "ISCAS85/c499", "ISCAS85/c880", "ISCAS85/c1355", "ISCAS85/c1908",
        "ISCAS85/c2670", "ISCAS85/c3540", "ISCAS85/c5315", "ISCAS85/c6288", "ISCAS85/c7552"}*//*;

    *//*static constexpr const std::array EPFL {
        "EPFL/priority", "EPFL/int2float", "EPFL/router", "EPFL/dec", "EPFL/cavlc", "EPFL/adder", "EPFL/priority", "EPFL/i2c",
        "EPFL/bar", "EPFL/max", "EPFL/sin", "EPFL/arbiter", "EPFL/voter", "EPFL/square"};*//*

    *//*static constexpr const uint64_t bench_select = fiction_experiments::fontes18 &
                                                   ~fiction_experiments::xor2_f &
                                                   ~fiction_experiments::t &
                                                   ~fiction_experiments::t_5 &
                                                   ~fiction_experiments::c17 &
                                                   ~fiction_experiments::majority_5_r1&
                                                   ~fiction_experiments::newtag&
                                                   ~fiction_experiments::clpl&
                                                   ~fiction_experiments::xor5_r1&
                                                   ~fiction_experiments::xor5_maj&
                                                   ~fiction_experiments::majority_5_r1;*//*

    //static constexpr const uint64_t bench_select = fiction_experiments::trindade16;
    static constexpr const uint64_t bench_select = fiction_experiments::ISCAS;

    //const std::string benchmark = "trindade16/FA";

    //for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    //for (const auto& benchmark : EPFL)
    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
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

        fiction::debug::write_dot_network(aig, fmt::format("{}{}.dot", benchmark, "logic_aig"));
        fiction::debug::write_dot_network(tech, fmt::format("{}{}.dot", benchmark, "logic_tech"));

        // perform technology mapping
        const auto mapped_network = mockturtle::map(aig, gate_lib, map_params);

        *//*mapped_network.foreach_gate(
            [&](const auto& n)
            {
                std::cout<<"Node "<<n<<std::endl;
            });

        std::cout<<"num_gates AIG"<<mapped_network.num_gates()<<std::endl;*//*

        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        //const auto layout_one = fiction::orthogonal<gate_layout>(mapped_network, {}, &st_tech_one);

        //fiction::debug::write_dot_layout(layout_one, "one");

        *//*const auto cell_level_lyt_o = apply_gate_library<qca_cell_clk_lyt, qca_one_library>(layout_one);

        write_qca_layout_svg(cell_level_lyt_o, fmt::format("{}{}.svg", benchmark, "_ortho"));*//*

        const auto layout_two = fiction::orthogonal_ordering_network<gate_layout>(mapped_network, {}, &st_tech_two);

        *//*const auto cell_level_lyt = apply_gate_library<qca_cell_clk_lyt, qca_one_library>(layout_two);

        write_qca_layout_svg(cell_level_lyt, fmt::format("{}{}.svg", benchmark, "_ortho_new"));*//*

        //fiction::debug::write_dot_layout(layout_two, "two");*/

        ortho_new_exp(benchmark, (st_tech_one.x_size-1), (st_tech_one.y_size-1),(st_tech_one.x_size-1) * (st_tech_one.y_size-1), (st_tech_two.x_size-1) * (st_tech_two.y_size-1), (st_tech_one.x_size-1) * (st_tech_one.y_size-1) - (st_tech_two.x_size-1) * (st_tech_two.y_size-1), mockturtle::to_seconds(st_tech_one.time_total),mockturtle::to_seconds(st_tech_two.time_total),
                      st_tech_one.num_gates, st_tech_two.num_gates, st_tech_one.num_wires, st_tech_two.num_wires);
    //}
    ortho_new_exp.save();
    ortho_new_exp.table();

}