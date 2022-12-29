//
// Created by Hien Benjamin on 15.12.2022.
//

#ifndef FICTION_ORTHO_MAJORITY_GATES_NETWORK_HPP
#define FICTION_ORTHO_MAJORITY_GATES_NETWORK_HPP
#include "ortho_new.hpp"

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif


namespace fiction
{
namespace detail
{

template <typename Lyt, typename Ntk>
class orthogonal_majority_network_impl
{
  public:
    orthogonal_majority_network_impl(const Ntk& src, const orthogonal_physical_design_params& p, orthogonal_physical_design_stats& st) :
            ntk{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(src)}},
            ps{p},
            pst{st}
    {}

    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        // compute a coloring
        const auto ctn = east_south_edge_coloring(ntk);

        mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)> node2pos{ctn.color_ntk};

        // instantiate the layout
        //Lyt layout{determine_layout_size<Lyt>(ctn), twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};
        Lyt layout{{1, 1, 1}, twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

        // reserve PI nodes without positions
        auto pi2node = reserve_input_nodes(layout, ctn.color_ntk);

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        tile<Lyt> latest_pos{1, 0};
        /*if(ctn.color_ntk.isFo_inv_flag()){
            ++latest_pos.x;
        }*/
        tile<Lyt> latest_pos_inputs{0, 0};

        latest_pos.x = latest_pos.x;

        latest_pos_inputs.x = latest_pos_inputs.x;

        //std::cout<<"Inverter_Flag"<<ctn.color_ntk.isFo_inv_flag()<<std::endl;

        std::cout<<"Numpis: "<<layout.num_pis()<<std::endl;

        std::cout<<"Numros: "<<layout.num_ros()<<std::endl;

        std::cout<<"Numcis: "<<layout.num_cis()<<std::endl;

        std::vector<std::pair<int, int>> resolve_rows;

        std::vector<std::pair<int, int>> resolve_columns;

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.size()), "[i] arranging layout: |{0}|"};
#endif

        ntk.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] const auto i)
            {
                // do not place constants
                if (!ctn.color_ntk.is_constant(n))
                {
                    //Resolve conflicts with Majority buffers
                    /*ctn.color_ntk.foreach_fanin(n, [&](const auto& fin)
                                                {
                                                    auto fin_del = ntk.get_node(fin);
                                                    if(ctn.color_ntk.color(n)==ctn.color_east || (ctn.color_ntk.color(n)==ctn.color_south && ctn.color_ntk.is_ci(fin_del)))
                                                    {
                                                        auto pre_t = static_cast<tile<Lyt>>(node2pos[fin_del]);
                                                        for (const auto item : resolve_rows) {
                                                            if (item.first == pre_t.y) {
                                                                pre_t = static_cast<tile<Lyt>>(wire_east(layout, pre_t, {item.second, item.first}));
                                                                node2pos[fin_del] = wire_south(layout, pre_t, {pre_t.x, latest_pos.y+1});
                                                                ++latest_pos.y;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    if(ctn.color_ntk.color(n)==ctn.color_south)
                                                    {
                                                        auto pre_t = static_cast<tile<Lyt>>(node2pos[fin_del]);

                                                        for (const auto item : resolve_columns) {
                                                            if (item.first == pre_t.x) {
                                                                pre_t = static_cast<tile<Lyt>>(wire_south(layout, pre_t, {item.first, item.second}));
                                                                node2pos[fin_del] = wire_east(layout, pre_t, {latest_pos.x + 1, pre_t.y});
                                                                ++latest_pos.x;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                });*/

                    // if node is a PI, move it to its correct position
                    if (ctn.color_ntk.is_pi(n))
                    {
                        node2pos[n] = layout.move_node(pi2node[n], {latest_pos_inputs});

                        // resolve conflicting PIs
                        ctn.color_ntk.foreach_fanout(
                            n,
                            [&ctn, &n, &layout, &node2pos, &latest_pos, &latest_pos_inputs](const auto& fon)
                            {
                                if (ctn.color_ntk.color(fon) == ctn.color_south)
                                {
                                    node2pos[n] =
                                        layout.create_buf(wire_east(layout, {latest_pos_inputs}, latest_pos), latest_pos);
                                    ++latest_pos.x;
                                }
                                std::cout<<n<<"Pi plaziert auf "<<"X:"<<latest_pos_inputs.x<<"Y:"<<latest_pos_inputs.y<<std::endl;
                                std::cout<<n<<"Pi wired to "<<"X:"<<latest_pos.x-1<<"Y:"<<latest_pos.y<<std::endl;

                                // PIs have only one fanout
                                return false;
                            });

                        ++latest_pos_inputs.y;
                        latest_pos.y=latest_pos_inputs.y;
                    }
                    /**NEW CODE
                     * MAJORITY GATES PLACEMENT
                     * **/
                    else if(const auto fc = fanins(ctn.color_ntk, n); ctn.color_ntk.is_maj(n) && fc.fanin_nodes.size()>2)
                    {
                        // const auto fc = fanins(ctn.color_ntk, n);
                        const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1], pre3 = fc.fanin_nodes[2];

                        auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                             pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]),
                             pre3_t = static_cast<tile<Lyt>>(node2pos[pre3]);

                        tile<Lyt> t{}, t1{}, t2{}, t3{};

                        auto maj_buf = majority_buffer(ctn.color_ntk, n);
                        /*for (int n_buf = 0; n_buf < maj_buf.size(); ++n_buf)
                        {
                            std::cout << "maj buf " << maj_buf[n_buf] << std::endl;
                        }*/
                        /**insert buffer for inputs colored south**//*
                        if(auto clr = ctn.color_ntk.color(n); ctn.color_ntk.is_ci(pre1) && clr != ctn.color_east)
                        {
                            pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {latest_pos.x + 1, pre1_t.y}));
                            ++latest_pos.x;
                        }
                        if(auto clr = ctn.color_ntk.color(n); ctn.color_ntk.is_ci(pre2) && clr != ctn.color_east)
                        {
                            pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {latest_pos.x + 1, pre2_t.y}));
                            ++latest_pos.x;
                        }
                        if(auto clr = ctn.color_ntk.color(n); ctn.color_ntk.is_ci(pre3) && clr != ctn.color_east)
                        {
                            pre3_t = static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {latest_pos.x + 1, pre3_t.y}));
                            ++latest_pos.x;
                        }*/
                        /**********************************************************************************************/

                        /**insert buffer when a fan-out has two same colored outputs
                        this should be resolved in the coloring**/
                        /*if(auto fos = fanouts(ctn.color_ntk, pre1); ctn.color_ntk.is_fanout(pre1) && fos.size()>1)
                        {
                            if(std::all_of(fos.cbegin(), fos.cend(),
                                            [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }) ||
                                std::all_of(fos.cbegin(), fos.cend(),
                                            [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_east; })  )
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                                pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {latest_pos.x + 1, pre1_t.y}));
                            }
                        }
                        if(auto fos = fanouts(ctn.color_ntk, pre2); ctn.color_ntk.is_fanout(pre2) && fos.size()>1)
                        {
                            if(std::all_of(fos.cbegin(), fos.cend(),
                                            [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }) ||
                                std::all_of(fos.cbegin(), fos.cend(),
                                            [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_east; })  )
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                                pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {latest_pos.x + 1, pre1_t.y}));
                            }
                        }
                        if(auto fos = fanouts(ctn.color_ntk, pre3); ctn.color_ntk.is_fanout(pre3) && fos.size()>1)
                        {
                            if(std::all_of(fos.cbegin(), fos.cend(),
                                            [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }) ||
                                std::all_of(fos.cbegin(), fos.cend(),
                                            [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_east; })  )
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                                pre3_t = static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {latest_pos.x + 1, pre1_t.y}));
                            }
                        }*/
                        /**********************************************************************************************/

                        // Majority gates placed south are just wired south and then placed east (this is due to the properties of the majority delay network)
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_null)
                        {
                            if (auto fos = fanouts(ctn.color_ntk, pre1);
                                ctn.color_ntk.is_fanout(pre1) && fos.size() > 1)
                            {
                                if (ctn.color_ntk.is_maj(fos[0]) && ctn.color_ntk.is_maj(fos[1]))
                                {
                                    pre1_t =
                                        static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {latest_pos.x + 1, pre1_t.y}));
                                    ++latest_pos.x;
                                }
                            }
                            if (auto fos = fanouts(ctn.color_ntk, pre2);
                                ctn.color_ntk.is_fanout(pre2) && fos.size() > 1)
                            {
                                if (ctn.color_ntk.is_maj(fos[0]) && ctn.color_ntk.is_maj(fos[1]))
                                {
                                    pre2_t =
                                        static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {latest_pos.x + 1, pre2_t.y}));
                                    ++latest_pos.x;
                                }
                            }
                            if (auto fos = fanouts(ctn.color_ntk, pre3);
                                ctn.color_ntk.is_fanout(pre3) && fos.size() > 1)
                            {
                                if (ctn.color_ntk.is_maj(fos[0]) && ctn.color_ntk.is_maj(fos[1]))
                                {
                                    pre3_t =
                                        static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {latest_pos.x + 1, pre3_t.y}));
                                    ++latest_pos.x;
                                }
                            }
                        }
                        if (const auto clr = ctn.color_ntk.color(n); clr != ctn.color_east)
                        {
                            pre1_t = static_cast<tile<Lyt>>(wire_south(layout, pre1_t, {pre1_t.x, latest_pos.y + 1}));
                            pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, pre1_t.y + 2}));
                            pre3_t = static_cast<tile<Lyt>>(wire_south(layout, pre3_t, {pre3_t.x, pre2_t.y + 2}));
                        }

                        // pre1_t is northern tile
                        // pre2_t is middle tile
                        // pre3_t is southern tile
                        if (pre2_t.y < pre1_t.y)
                        {
                            std::swap(pre1_t, pre2_t);
                            std::swap(maj_buf[0], maj_buf[1]);
                        }
                        if (pre3_t.y < pre1_t.y)
                        {
                            std::swap(pre1_t, pre3_t);
                            std::swap(maj_buf[0], maj_buf[2]);
                        }
                        if (pre3_t.y < pre2_t.y)
                        {
                            std::swap(pre2_t, pre3_t);
                            std::swap(maj_buf[1], maj_buf[2]);
                        }

                        /**wire
                             * majority_buffer
                             * east**/
                             int local_resolve_row;
                        for(int path_n = maj_buf.size()-1; path_n >= 0 ; --path_n)
                        {
                            if(maj_buf[path_n] >= 1)
                            {
                                if(path_n == 2)
                                {
                                    //For this case we need RESOLVE for nodes getting wired east but are blocked by the Buffer
                                    pre3_t = static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {latest_pos.x + 1, pre3_t.y}));
                                    const auto uno = pre3_t.y+1;
                                    const auto due = pre3_t.x;
                                    const std::pair<int, int> row_resolve_to_column = {uno, due};
                                    resolve_rows.push_back(row_resolve_to_column);


                                    auto pre_clock = layout.get_clock_number({pre3_t});
                                    for(int iter = maj_buf[path_n]; iter > 0; --iter)
                                    {
                                        pre3_t =
                                            static_cast<tile<Lyt>>(buffer_east_case_one(layout, pre3_t, pre_clock));
                                    }

                                    latest_pos.x = pre3_t.x+1;

                                    //CASE the other buffers also
                                    if(maj_buf[1] > 0 || maj_buf[0] > 0)
                                    {
                                        pre3_t = static_cast<tile<Lyt>>(wire_south(layout, pre3_t, {pre3_t.x, pre3_t.y+2}));

                                        local_resolve_row = pre3_t.x + 1;
                                        if(maj_buf[1] == 0)
                                        {
                                            pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {local_resolve_row + 1, pre2_t.y}));
                                            pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, pre2_t.y+2}));
                                            ++latest_pos.x;
                                        }
                                    }
                                }
                                else if(path_n == 1)
                                {
                                    /*if(maj_buf[2] >= 1)
                                    {
                                        pre3_t = static_cast<tile<Lyt>>(wire_south(layout, pre3_t, {pre3_t.x, pre3_t.y+2}));
                                    }
                                    else
                                    {
                                        for (const auto item : resolve_rows) {
                                            if (item.first == pre3_t.y) {
                                                pre3_t = static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {item.second, item.first}));
                                                pre3_t = static_cast<tile<Lyt>>(wire_south(layout, pre3_t, {pre3_t.x, latest_pos.y}));
                                                ++latest_pos.y;
                                                break;
                                            }
                                        }
                                    }*/

                                    //For this case we need RESOLVE for nodes getting wired east but are blocked by the Buffer
                                    pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {latest_pos.x + 1, pre2_t.y}));
                                    const auto uno = pre2_t.y+1;
                                    const auto due = pre2_t.x;
                                    const std::pair<int, int> row_resolve_to_column = {uno, due};
                                    resolve_rows.push_back(row_resolve_to_column);


                                    auto pre_clock = layout.get_clock_number({pre2_t});
                                    for(int iter = maj_buf[path_n]; iter > 0; --iter)
                                    {
                                        pre2_t =
                                            static_cast<tile<Lyt>>(buffer_east_case_one(layout, pre2_t, pre_clock));
                                    }
                                    pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, pre2_t.y+2}));

                                    if(maj_buf[2] == 0)
                                    {
                                        pre3_t = static_cast<tile<Lyt>>(wire_south(layout, pre3_t, {pre3_t.x, pre3_t.y+2}));
                                    }

                                    latest_pos.x = pre2_t.x+1;
                                }
                                else
                                {
                                    /*if(maj_buf[1] >= 1)
                                    {
                                        pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, pre2_t.y+2}));
                                    }
                                    else
                                    {
                                        for (const auto item : resolve_rows) {
                                            if (item.first == pre2_t.y) {
                                                pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {item.second, item.first}));
                                                pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, latest_pos.y}));
                                                ++latest_pos.y;
                                                break;
                                            }
                                        }
                                    }*/


                                    pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {latest_pos.x + 1, pre1_t.y}));
                                    const auto uno = pre1_t.y+1;
                                    const auto due = pre1_t.x;
                                    const std::pair<int, int> row_resolve_to_column = {uno, due};
                                    resolve_rows.push_back(row_resolve_to_column);


                                    auto pre_clock = layout.get_clock_number({pre1_t});
                                    for(int iter = maj_buf[path_n]; iter > 0; --iter)
                                    {
                                        pre1_t =
                                            static_cast<tile<Lyt>>(buffer_east_case_one(layout, pre1_t, pre_clock));
                                    }
                                    pre1_t = static_cast<tile<Lyt>>(wire_south(layout, pre1_t, {pre1_t.x, pre1_t.y+2}));

                                    latest_pos.x = pre1_t.x+1;

                                    //because buffer is 2 tiles wide in y-direction
                                    ++latest_pos.y;
                                }
                            }
                        }
                        /****************************************************************************************************************/


                        //Dimensions of majority gate are [4, 7]
                        //Three starting points of the majority input network

                        /* 1 2 3 4 1
                         * 2 3 4 1 2
                         * 3 4 3 2 3
                         * . 1 4 1 4
                         * . 2 1 M 1
                         * . 3 4 3 2
                         * . . . 4 1
                         */

                        if(pre3_t.y - pre2_t.y < 4)
                        {
                            //pre2_t gives y coordinates
                            t1 = {latest_pos.x + 3, pre2_t.y + 3};
                            t2 = {latest_pos.x + 2, pre2_t.y    };
                            t3 = {latest_pos.x    , pre2_t.y + 4};
                        }
                        else
                        {
                            //pre3_t gives y coordinates
                            t1 = {latest_pos.x + 3, pre3_t.y - 1};
                            t2 = {latest_pos.x + 2, pre3_t.y - 4};
                            t3 = {latest_pos.x    , pre3_t.y    };
                        }
                        // coordinates of the majority gate
                        t = {t2.x, t2.y + 3};

                        //wire east
                        pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t1.x + 1, pre1_t.y}));
                        pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {t2.x + 1, pre2_t.y}));
                        pre3_t = static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {t3.x + 1, pre3_t.y}));

                        // Majority gates placed east need to be placed south as well because of the non-uniformity of the clocking
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            if(t.y < latest_pos_inputs.y)
                            {
                                t1 = {t1.x, t1.y + latest_pos_inputs.y-2};
                                t2 = {t2.x, t2.y + latest_pos_inputs.y-2};
                                t3 = {t2.x, t3.y + latest_pos_inputs.y-2};

                                t = {t.x, t.y + latest_pos_inputs.y-2};
                            }
                            else if(latest_pos.y > 3)
                            {
                                t = {t.x, latest_pos.y+2};

                                t1 = {t1.x, t.y};
                                t2 = {t2.x, t.y-3};
                                t3 = {t2.x, t.y+1};
                            }
                        }

                        //Delay Network
                        //T1
                        pre1_t = static_cast<tile<Lyt>>(wire_south(layout, pre1_t, {pre1_t.x, t1.y+1}));
                        //end
                        //T2
                        pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t2.y + 2}));
                        auto pre_clock = layout.get_clock_number({pre2_t});
                        pre2_t = static_cast<tile<Lyt>>(wire_west(layout, pre2_t, {pre2_t.x - 2, pre2_t.y}));
                        layout.assign_clock_number({pre2_t.x,pre2_t.y,0}, pre_clock+1);
                        pre_clock = layout.get_clock_number({pre2_t});
                        pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, pre2_t.y + 2}));
                        layout.assign_clock_number({pre2_t.x,pre2_t.y,0}, pre_clock+1);
                        pre_clock = layout.get_clock_number({pre2_t});
                        pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {pre2_t.x + 2, pre2_t.y}));
                        layout.assign_clock_number({pre2_t.x,pre2_t.y,0}, pre_clock+1);
                        //end
                        //T3
                        if(t3.y != pre3_t.y){
                            pre3_t = static_cast<tile<Lyt>>(wire_south(layout, pre3_t, {pre3_t.x, t3.y+1}));
                            pre3_t = static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {pre3_t.x + 2, pre3_t.y}));
                        }
                        pre_clock = layout.get_clock_number({pre3_t});
                        pre3_t = static_cast<tile<Lyt>>(wire_north(layout, pre3_t, {pre3_t.x, pre3_t.y - 2}));
                        layout.assign_clock_number({pre3_t.x,pre3_t.y,0}, pre_clock+1);
                        pre_clock = layout.get_clock_number({pre3_t});
                        //end

                        node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre2_t, pre3_t, pre1_t);
                        std::cout<<n<<"Maj plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                        std::cout<<"Pre 1: "<<pre1<<std::endl;
                        std::cout<<"Pre 2: "<<pre2<<std::endl;
                        std::cout<<"Pre 3: "<<pre3<<std::endl;
                        layout.assign_clock_number({t.x,t.y,0}, pre_clock+1);

                        pre_clock = layout.get_clock_number({t});
                        t = static_cast<tile<Lyt>>(wire_south(layout, t, {t.x, t.y + 3}));
                        layout.assign_clock_number({t.x,t.y-1,0}, pre_clock+1);
                        layout.assign_clock_number({t.x,t.y,0}, pre_clock+2);
                        pre_clock = layout.get_clock_number({t});
                        t = static_cast<tile<Lyt>>(wire_east(layout, t, {t.x +2, t.y}));
                        layout.assign_clock_number({t.x,t.y,0}, pre_clock+1);
                        t = static_cast<tile<Lyt>>(wire_north(layout, t, {t.x, t.y -2}));
                        t = static_cast<tile<Lyt>>(wire_east(layout, t, {t.x +2, t.y}));

                        node2pos[n] = static_cast<mockturtle::signal<Lyt>>(t);

                        std::cout<<"Color MAj gate: "<<(ctn.color_ntk.color(n))<<std::endl;


                        latest_pos.x = t.x + 1;
                        latest_pos.y = t.y + 2;

                        std::cout<<"latest_pos.x: "<<latest_pos.x<<std::endl;
                        std::cout<<"latest_pos.y: "<<latest_pos.y<<std::endl;
                        /*********************************************************************************************************************************************/

                    }
                    // if n has only one fanin
                    else if (const auto fc = fanins(ctn.color_ntk, n); fc.fanin_nodes.size() == 1)
                    {
                        const auto& pre = fc.fanin_nodes[0];

                        const auto pre_t = static_cast<tile<Lyt>>(node2pos[pre]);

                        /*if(ctn.color_ntk.is_fanout(n) && ctn.color_ntk.is_pi(pre)){
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, ctn.color_east);
                            ++latest_pos.y;
                        }*/

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            auto insert_position = latest_pos.x;
                            /*if(ntk.isFo_inv_flag() && ntk.is_inv(n) && ntk.is_pi(pre)){
                                insert_position = 1;
                                --latest_pos.x;
                                std::cout<<"INV";
                            }*/
                            /*oder ist inverter und nächster knoten ist pi*/
                            if(ctn.color_ntk.is_fanout(n) && ctn.color_ntk.is_pi(pre))
                            {
                                ++latest_pos.y;
                            }
                            const tile<Lyt> t{insert_position, pre_t.y};

                            if(ctn.color_ntk.is_fanout(n)){
                                std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            }else{
                                std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            }

                            std::cout<<n<<"Pre"<<pre<<std::endl;
                            std::cout<<n<<"color: "<<"east"<<std::endl;
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            if((ctn.color_ntk.is_inv(n) ||ctn.color_ntk.is_fanout(n)) && latest_pos.y<latest_pos_inputs.y){
                                const tile<Lyt> t{pre_t.x, latest_pos_inputs.y};
                                if(ctn.color_ntk.is_fanout(n)){
                                    std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }else{
                                    std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }
                                std::cout<<n<<"Pre"<<pre<<std::endl;
                                std::cout<<n<<"color: "<<"south"<<std::endl;
                                node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                                latest_pos.y = t.y+1;
                            }
                            else
                            {
                                const tile<Lyt> t{pre_t.x, latest_pos.y};
                                if(ctn.color_ntk.is_fanout(n)){
                                    std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }else{
                                    std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }
                                std::cout<<n<<"Pre"<<pre<<std::endl;
                                std::cout<<n<<"color: "<<"south"<<std::endl;
                                node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                                ++latest_pos.y;
                            }
                        }
                        else
                        {
                            // single fanin nodes should not be colored null
                            assert(false);
                        }
                    }
                    else  // if node has two fanins (or three fanins with one of them being constant)
                    {
                        const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];

                        auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                             pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

                        tile<Lyt> t{};

                        auto maj_buf = majority_buffer(ctn.color_ntk, n);

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            // make sure pre1_t is the northwards tile
                            if (pre2_t.y < pre1_t.y){
                                std::swap(pre1_t, pre2_t);
                                std::swap(maj_buf[0], maj_buf[1]);
                            }


                            // use larger y position of predecessors
                            t = {latest_pos.x, pre2_t.y};

                            // each 2-input gate has one incoming bent wire
                            pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));

                            ++latest_pos.x;
                            if(latest_pos.y < latest_pos_inputs.y){
                                latest_pos.y = t.y+1;
                            }

                            /**wire
                             * majority_buffer
                             * east**/
                            for(int path_n = 0; path_n < maj_buf.size(); ++path_n)
                            {
                                if(maj_buf[path_n] >= 1)
                                {
                                    if(path_n == 0)
                                    {
                                        std::cout << "CASE: ONE" << '\n';
                                        /*For this case we need RESOLVE for nodes getting wired east but are blocked by the Buffer*/
                                        const auto uno = pre1_t.y+1;
                                        const auto due = pre1_t.x+1;
                                        const std::pair<int, int> row_resolve_to_column = {uno, due};
                                        resolve_rows.push_back(row_resolve_to_column);

                                        pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {pre1_t.x +2, pre1_t.y}));


                                        auto pre_clock = layout.get_clock_number({pre1_t});
                                        for(int iter = maj_buf[path_n]; iter > 0; --iter)
                                        {
                                            pre1_t =
                                                static_cast<tile<Lyt>>(buffer_east_case_one(layout, pre1_t, pre_clock));
                                        }

                                        //pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {pre1_t.x + 5, pre1_t.y}));


                                        t = {pre1_t.x, t.y};

                                        latest_pos.x = pre1_t.x+1;
                                    }
                                    else
                                    {
                                        std::cout << "CASE: TWO" << '\n';

                                        pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {t.x + 1, pre2_t.y}));
                                        const auto uno = pre2_t.y+1;
                                        const auto due = pre2_t.x;
                                        const std::pair<int, int> row_resolve_to_column = {uno, due};
                                        resolve_rows.push_back(row_resolve_to_column);

                                        auto pre_clock = layout.get_clock_number({pre2_t});
                                        for(int iter = maj_buf[path_n]; iter > 0; --iter)
                                        {
                                            pre2_t =
                                                static_cast<tile<Lyt>>(buffer_east_case_two(layout, pre2_t, pre_clock));
                                        }

                                        t = {pre2_t.x + 1, t.y};

                                        pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));

                                        latest_pos.x = t.x+1;

                                        //because buffer is 2 tiles wide in y-direction
                                        ++latest_pos.y;
                                    }
                                }
                            }
                            /****************************************************************************************************************/

                            std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                            std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                            std::cout<<n<<"color: "<<"east"<<std::endl;

                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            auto pre_fo = pre2;
                            // pre2_t is the westwards tile
                            if (pre2_t.x > pre1_t.x){
                                std::swap(pre1_t, pre2_t);
                                std::swap(maj_buf[0], maj_buf[1]);
                                pre_fo = pre1;
                            }

                            /**NEW CODE
                             * !!new south wire option
                             * **/
                            // check if pre1_t is now also the northwards tile
                            if (pre1_t.y < pre2_t.y && !ctn.color_ntk.is_fanout(pre_fo) && maj_buf[0] == 0 && maj_buf[1] == 0)
                            {
                                if(pre2_t.x == pre1_t.x)
                                {
                                    // node can be placed on y position of pre2_t
                                    std::cout<<"Neue south wires für: "<<n<<std::endl;

                                    // use larger x position of predecessors
                                    t = {latest_pos.x, pre2_t.y};
                                    /*std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;*/
                                    ++latest_pos.x;
                                }
                                else
                                {
                                    // node can be placed on y position of pre2_t
                                    std::cout<<"Neue south wires für: "<<n<<std::endl;

                                    // use larger x position of predecessors
                                    t = {pre1_t.x, pre2_t.y};
                                    /*std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;*/
                                }
                                if(pre2_t.y+1>latest_pos.y)
                                {
                                    latest_pos.y =pre2_t.y+1;
                                }
                            }
                            /**!!**************************************************************************************/
                            else
                            {
                                if(latest_pos.y<latest_pos_inputs.y)
                                {
                                    // use larger x position of predecessors
                                    t = {pre1_t.x, latest_pos_inputs.y};
                                    // each 2-input gate has one incoming bent wire
                                    pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                                    latest_pos.y = t.y+1;
                                }
                                else
                                {
                                    // use larger x position of predecessors
                                    t = {pre1_t.x, latest_pos.y};

                                    // each 2-input gate has one incoming bent wire
                                    pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                                    ++latest_pos.y;
                                }
                            }

                            /**wire
                             * majority_buffer
                             * south**/
                            for(int path_n = 0; path_n < maj_buf.size(); ++path_n)
                            {
                                if(maj_buf[path_n] >= 1)
                                {
                                    if(path_n == 0)
                                    {
                                        pre1_t = static_cast<tile<Lyt>>(wire_south(layout, pre1_t, {pre1_t.x, t.y + 2}));
                                        //std::cout<<"wire to "<<"X: "<<pre1_t.x<<"Y: "<<pre1_t.y<<std::endl;
                                        const auto uno = pre1_t.x + 1;
                                        const auto due = pre1_t.y + 1;
                                        const std::pair<int, int> column_resolve_to_row = {uno, due};
                                        resolve_columns.push_back(column_resolve_to_row);

                                        auto pre_clock = layout.get_clock_number({pre1_t});
                                        for(int iter = maj_buf[path_n]; iter > 0; --iter)
                                        {
                                            pre1_t = static_cast<tile<Lyt>>(
                                                buffer_south_case_one(layout, pre1_t, pre_clock));
                                        }
                                        //std::cout << "TYPE: " << typeid(pre_clock).name() << '\n';
                                        pre1_t = static_cast<tile<Lyt>>(wire_south(layout, pre1_t, {pre1_t.x, pre1_t.y + 4}));

                                        t = {t.x, pre1_t.y+1};

                                        pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                                        latest_pos.y = t.y+1;

                                        //because buffer is 2 tiles wide in x-direction
                                        ++latest_pos.x;
                                        std::cout<<"latest.pos "<<"X: "<<latest_pos.x<<"Y: "<<latest_pos.y<<std::endl;
                                    }
                                    else
                                    {
                                        //For this case we need RESOLVE for nodes getting wired east but are blocked by the Buffer
                                        const auto uno = pre2_t.x + 1;
                                        const auto due = pre2_t.y + 1;
                                        const std::pair<int, int> column_resolve_to_row = {uno, due};
                                        resolve_columns.push_back(column_resolve_to_row);

                                        pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, pre2_t.y + 2}));
                                        //std::cout<<"wire to "<<"X: "<<pre2_t.x<<"Y: "<<pre2_t.y<<std::endl;

                                        auto pre_clock = layout.get_clock_number({pre2_t});
                                        for(int iter = maj_buf[path_n]; iter > 0; --iter)
                                        {
                                            pre2_t = static_cast<tile<Lyt>>(
                                                buffer_south_case_two(layout, pre2_t, pre_clock));
                                        }


                                        t = {t.x, pre2_t.y};
                                        latest_pos.y = pre2_t.y+1;
                                        std::cout<<n<<"T "<<"X: "<<t.x<<"Y: "<<t.y<<std::endl;
                                    }
                                }
                            }
                            std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                            std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                            std::cout<<n<<"color: "<<"south"<<std::endl;
                            /****************************************************************************************************************/


                        }
                        // n is colored null; corner case
                        else
                        {
                            // make sure pre1_t has an empty tile to its east and pre2_t to its south
                            std::swap(pre1_t, pre2_t);
                            if (!layout.is_empty_tile(layout.east(pre1_t)) ||
                                !layout.is_empty_tile(layout.south(pre2_t)))
                                std::swap(pre1_t, pre2_t);

                            const auto ttt = layout.get_node(layout.east(pre1_t));

                            t = latest_pos;

                            std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                            std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                            std::cout<<n<<"color: "<<"southeast"<<std::endl;

                            // both wires have one bent
                            pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));
                            pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                            ++latest_pos.x;
                            ++latest_pos.y;
                        }

                        node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre1_t, pre2_t, fc.constant_fanin);
                    }
                    if (ctn.color_ntk.is_po(n))
                    {
                        if (!is_eastern_po_orientation_available(ctn, n))
                        {++latest_pos.y;}
                    }
                }

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
            });

        /**********************************************************Begin: Place Pos***************************************************************/
        //Since the layout size is only known after placing all gates, the POs have to be placed after the main algorithm
        //
        //
        ctn.color_ntk.foreach_po(
            [&](const auto& po){
                const auto n_s = node2pos[po];

                tile<Lyt> po_tile{};

                // determine PO orientation
                if (is_eastern_po_orientation_available(ctn, po))
                {
                    po_tile = static_cast<tile<Lyt>>(n_s);
                    //++po_tile.x;
                    //++latest_pos.x;
                    layout.resize({latest_pos.x, latest_pos.y-1, 1});
                    //layout.resize({50, 50, 1});
                }
                else
                {
                    po_tile = static_cast<tile<Lyt>>(n_s);
                    //++latest_pos.y;
                    po_tile = static_cast<tile<Lyt>>(wire_south(layout, po_tile, {po_tile.x, po_tile.y + 2}));
                    //++po_tile.y;
                    layout.resize({latest_pos.x, latest_pos.y-1, 1});
                    //layout.resize({50, 50, 1});
                }

                // check if PO position is located at the border
                if (layout.is_at_eastern_border({po_tile.x + 1, po_tile.y}))
                {
                    ++po_tile.x;
                    layout.create_po(n_s,
                                     ctn.color_ntk.has_output_name(po_counter) ?
                                         ctn.color_ntk.get_output_name(po_counter++) :
                                         fmt::format("po{}", po_counter++),
                                     po_tile);
                }
                // place PO at the border and connect it by wire segments
                else
                {
                    const tile<Lyt> anker{po_tile};

                    po_tile = layout.eastern_border_of(po_tile);

                    layout.create_po(wire_east(layout, anker, po_tile),
                                     ctn.color_ntk.has_output_name(po_counter) ?
                                         ctn.color_ntk.get_output_name(po_counter++) :
                                         fmt::format("po{}", po_counter++),
                                     po_tile);
                }
                //std::cout<<po<<"PO plaziert auf"<<"X:"<<po_tile.x<<"Y:"<<po_tile.y<<std::endl;
            });
        /**********************************************************End: Place Pos***************************************************************/

        int crossing_count{0};
        layout.foreach_tile(
            [&layout, &crossing_count](const auto& t)
            {
                const auto nde = layout.get_node(t);
                if(layout.is_wire(nde))
                {
                    if(t.z == 1)
                    {
                        ++crossing_count;
                    }
                }
            });
        std::cout<<"Crossing_Num: "<<crossing_count<<std::endl;

        // restore possibly set signal names
        restore_names(ctn.color_ntk, layout, node2pos);

        // statistical information
        pst.x_size    = layout.x() + 1;
        pst.y_size    = layout.y() + 1;
        pst.num_gates = layout.num_gates();
        pst.num_wires = layout.num_wires();

        std::cout<<"latest X: "<<latest_pos.x<<std::endl;
        std::cout<<"latest Y: "<<latest_pos.y<<std::endl;

        return layout;
    }

  private:
    mockturtle::topo_view<mockturtle::fanout_view<mockturtle::names_view<technology_network>>> ntk;

    orthogonal_physical_design_params ps;
    orthogonal_physical_design_stats& pst;

    uint32_t po_counter{0};
};

}  // namespace detail


template <typename Lyt, typename Ntk>
Lyt orthogonal_majority_network(const Ntk& ntk, orthogonal_physical_design_params ps = {},
                              orthogonal_physical_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyway, therefore,
                                                 // this is the only relevant check here

    // check for input degree
    if (has_high_degree_fanin_nodes(ntk, 3))
    {
        throw high_degree_fanin_exception();
    }

    orthogonal_physical_design_stats  st{};
    detail::orthogonal_majority_network_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction
#endif  // FICTION_ORTHO_MAJORITY_GATES_NETWORK_HPP
