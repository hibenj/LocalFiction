//
// Created by Hien Benjamin on 12.09.2022.
//
#include "fiction_experiments.hpp"
#include "fiction/algorithms/physical_design/ortho_new.hpp"
//#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "mockturtle/io/verilog_reader.hpp"

#include <iostream>

using namespace fiction;

int main()
{
    experiments::experiment<std::string, uint32_t, uint32_t, double, double>
        ortho_new_exp( "Ortho_majority_placement",
                      "Benchmark",
                      "State of the art size",
                      "ortho_new size",
                      "State of the art runtime",
                      "ortho_new runtime");

    fiction::orthogonal_physical_design_stats st_aig;
    fiction::orthogonal_physical_design_stats st_tech;

    static constexpr const uint64_t bench_select = fiction_experiments::mux21;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print( "[i] processing {}\n", benchmark );

        mockturtle::aig_network aig;
        fiction::technology_network tech;

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


        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        //const auto layout_one = orthogonal<gate_layout>(tech, {}, &st_tech);

        //const auto layout_two = fiction::orthogonal_new<gate_layout>(tech, {}, &st_tech);

        //ortho_new_exp(benchmark, st_aig.x_size * st_aig.y_size, st_tech.x_size * st_tech.y_size, mockturtle::to_seconds(st_aig.time_total),mockturtle::to_seconds(st_tech.time_total));
    }

}