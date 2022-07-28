//
// Created by Hien Benjamin on 24.05.2022.
//

#ifndef FICTION_ORTHO_NEW_HPP
#define FICTION_ORTHO_NEW_HPP

#include "fiction/algorithms/network_transformation/fanout_inverter_balancing.hpp"
#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/networks/views/topo_view_input_sort.hpp"
#include "fiction/networks/views/reverse_view.hpp"

int create_ten(){
    int a = 10;
    return a;
};


namespace fiction
{
namespace detail
{

/*This is needed for the delay network*/
template <typename Lyt>
mockturtle::signal<Lyt> wire_west(Lyt& lyt, const tile<Lyt>& src, const tile<Lyt>& dest)
{
    auto a = static_cast<mockturtle::signal<Lyt>>(src);

    for (auto x = src.x - 1; x > dest.x; --x)
    {
        auto t = tile<Lyt>{x, src.y, 0};
        if (!lyt.is_empty_tile(t))  // crossing case
        {
            t = lyt.above(t);
        }

        a = lyt.create_buf(a, t);
    }

    return a;
}
/*This is needed for the delay network*/
template <typename Lyt>
mockturtle::signal<Lyt> wire_north(Lyt& lyt, const tile<Lyt>& src, const tile<Lyt>& dest)
{
    auto a = static_cast<mockturtle::signal<Lyt>>(src);

    for (auto y = src.y - 1; y > dest.y; --y)
    {
        auto t = tile<Lyt>{src.x, y, 0};
        if (!lyt.is_empty_tile(t))  // crossing case
        {
            t = lyt.above(t);
        }

        a = lyt.create_buf(a, t);
    }

    return a;
}

template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> connect_and_place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                                          tile<Lyt> pre1_t, tile<Lyt> pre2_t, tile<Lyt> pre3_t)
{
    //pre1_t is northern tile
    //pre2_t is western tile
    //pre3_t is eastern tile
    if (pre2_t.y < pre1_t.y)
        std::swap(pre1_t, pre2_t);
    if (pre3_t.y < pre1_t.y)
        std::swap(pre1_t, pre3_t);
    if (pre3_t.x < pre2_t.x)
        std::swap(pre2_t, pre3_t);

    return place(lyt, t, ntk, n, wire_south(lyt, pre1_t, t), wire_east(lyt, pre2_t, t), wire_west(lyt, pre3_t, t));
}

/*This is needed for the conditional coloring*/
template <typename Ntk>
void paint_edge_if(const coloring_container<Ntk>& ctn,const mockturtle::edge<out_of_place_edge_color_view<Ntk>>& e, uint32_t clr)
{
    if (ctn.color_ntk.edge_color(e) != ctn.color_null)
    {
        return;
    }
    ctn.color_ntk.paint_edge(e, clr);
}
/*This is needed for the conditional coloring*/
template <typename Ntk>
void paint_if( const coloring_container<Ntk>& ctn, mockturtle::node<Ntk> const& n, uint32_t color )
{
    if (ctn.color_ntk.color(n) != ctn.color_null)
    {
        return;
    }
    ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, color);
}
/*This is needed for the conditional coloring*/
template <typename Ntk>
void recursevly_paint_fan_outs( const coloring_container<Ntk>& ctn, mockturtle::node<Ntk> const& n)
{
    if (ctn.color_ntk.color(n) != ctn.color_null)
    {
        return;
    }
    auto color = ctn.color_east;
    if(ctn.color_ntk.is_fanout(n)){
        paint_if(ctn, n, color);
    }
    ctn.color_ntk.foreach_fanout(n, [&](const auto& nd){
                                     if(ctn.color_ntk.is_fanout(n))
                                     {
                                         recursevly_paint_fan_outs(ctn, n);
                                     }});

}
/*Conditional coloring*/
template <typename Ntk>
coloring_container<Ntk> new_east_south_edge_coloring(const Ntk& ntk) noexcept
{
    coloring_container<Ntk> ctn{ntk};
    reverse_view            rtv{ntk};  // reverse order of nodes

    /*Coloring for Input Network*/
    bool fo_node = false;
    bool node_pi = false;
    bool node_fo = false;
    bool inv_flag = false;

    std::vector<typename Ntk::node> new_output_node;

    //Paint majority gates east
    /**NEW CODE MAJORITY GATES**/
    ntk.foreach_gate([&](const auto& nd){

        if(ntk.is_maj(nd))
        {
            auto color = ctn.color_east;
            const auto finc_fo = fanin_edges(ctn.color_ntk, nd);
            std::for_each(finc_fo.fanin_edges.cbegin(), finc_fo.fanin_edges.cend(),
                          [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
            paint_if(ctn, nd, color);

            ntk.foreach_fanout(nd, [&ntk, &nd, &ctn](const auto& fon)
                               {
                                   auto color = ctn.color_east;
                                   const auto finc_fo = fanin_edges(ctn.color_ntk, nd);
                                   std::for_each(finc_fo.fanin_edges.cbegin(), finc_fo.fanin_edges.cend(),
                                                 [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                   paint_if(ctn, nd, color);
                               });
        }
    });


    ntk.foreach_pi(
        [&](const auto& nd){
            ntk.foreach_fanout(
                nd,
                [&](const auto& fon)
                {
                    fo_node = false;
                    node_pi = false;
                    node_fo = false;
                    inv_flag = false;
                    /*Ignore Inverters*/
                    new_output_node.clear();
                    new_output_node.push_back(fon);
                    if (ntk.is_inv(fon))
                    {
                        ntk.foreach_fanout(fon,
                                           [&](const auto& fon_inv) {
                                               new_output_node[0] = fon_inv;

                                               /*COLOR INVERTER EAST*/
                                               auto color = ctn.color_east;
                                               const auto finc = fanin_edges(ctn.color_ntk, fon);
                                               std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                                             [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                           });
                    }
                    if (ntk.is_fanout(new_output_node[0])){
                        mockturtle::node<Ntk> safe_node = new_output_node[0];
                        fo_node = true;

                        /*COLOR FAN-OUT EAST*/
                        auto color = ctn.color_east;
                        const auto finc = fanin_edges(ctn.color_ntk, new_output_node[0]);
                        std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });

                        new_output_node.clear();
                        ntk.foreach_fanout(safe_node,
                                           [&](const auto& fon) {
                                               if (ntk.is_inv(fon))
                                               {
                                                   inv_flag = true;
                                                   /*Inverter Flag*/
                                                   ntk.foreach_fanout(fon,
                                                                      [&](const auto& fon_inv) {new_output_node.insert(new_output_node.begin(), fon_inv);});
                                               }
                                               else
                                                   /*COLOR NODE SOUTH AND COLOR THE INCOMING EDGES OF THIS NODE ALSO SOUTH*/
                                                   new_output_node.push_back(fon);
                                               /*COLOR THE OTHER OUTGOING EDGE OF THE FAN-OUT EAST*/
                                               /*IF the node is an inverter color next edge also east*/
                                           });
                    }

                    /*The Fan-out has to be connected to 1.Fan-out nodes of Inputs or 2.Inputs*/
                    /*Has to be rewritten for Inputs with Fan-outs higher than two*/
                    bool already_painted = false;
                    bool all_inputs = false;
                    for(int i = 0; i <new_output_node.size(); ++i)
                    {
                        ntk.foreach_fanin(
                            new_output_node[i],
                            [&](const auto& fi)
                            {
                                auto fin_inp = ntk.get_node(fi);
                                /*Ignore Inverters*/
                                if (ntk.is_inv(fin_inp))
                                {
                                    auto color = ctn.color_east;
                                    const auto fin_inv = fanin_edges(ctn.color_ntk, fin_inp);
                                    std::for_each(fin_inv.fanin_edges.cbegin(), fin_inv.fanin_edges.cend(),
                                                  [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                    paint_if(ctn, fin_inp, color);
                                    const auto fis_inv = fanins(ntk, fin_inp);
                                    fin_inp            = fis_inv.fanin_nodes[0];
                                }
                                /*1*/
                                if (ntk.fanout_size(fin_inp) >= 2)
                                {
                                    ntk.foreach_fanin(
                                        fin_inp,
                                        [&](const auto& fin)
                                        {
                                            auto fin_inp_sec = ntk.get_node(fin);
                                            /*Ignore Inverters*/
                                            if (ntk.is_inv(fin_inp_sec))
                                            {
                                                const auto fis_inv = fanins(ntk, fin_inp_sec);
                                                fin_inp_sec        = fis_inv.fanin_nodes[0];
                                            }
                                            if (ntk.is_ci(fin_inp_sec) == true &&
                                                fin_inp_sec != nd)
                                            {
                                                if(fo_node && !already_painted){
                                                    if (inv_flag)
                                                    {
                                                        auto color = ctn.color_south;
                                                        const auto finc_fo = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                        std::for_each(finc_fo.fanin_edges.cbegin(), finc_fo.fanin_edges.cend(),
                                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                        paint_if(ctn, new_output_node[i], color);

                                                        i = 1 - i;

                                                        /*Color other fan-out south*/
                                                        color = ctn.color_south;
                                                        const auto finc_other = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                        std::for_each(finc_other.fanin_edges.cbegin(), finc_other.fanin_edges.cend(),
                                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                        paint_if(ctn, new_output_node[i], color);

                                                        already_painted = true;
                                                    }
                                                    else
                                                    {
                                                        /*Color first fan-out node east*/
                                                        auto color = ctn.color_east;
                                                        const auto finc_fo = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                        std::for_each(finc_fo.fanin_edges.cbegin(), finc_fo.fanin_edges.cend(),
                                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                        paint_if(ctn, new_output_node[i], color);

                                                        i = 1 - i;

                                                        /*Color other fan-out south*/
                                                        color = ctn.color_south;
                                                        const auto finc_other = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                        std::for_each(finc_other.fanin_edges.cbegin(), finc_other.fanin_edges.cend(),
                                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                        paint_if(ctn, new_output_node[i], color);

                                                        already_painted = true;
                                                    }

                                                }
                                                /*else if(!already_painted){
                                                    auto color = ctn.color_south;
                                                    const auto finc_other = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                    std::for_each(finc_other.fanin_edges.cbegin(), finc_other.fanin_edges.cend(),
                                                                  [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                    ctn.color_ntk.paint(mockturtle::node<Ntk>{new_output_node[i]}, color);

                                                    already_painted = true;
                                                }*/
                                                node_fo = true;
                                                all_inputs = true;
                                            }
                                        });
                                }
                                /*2*/
                                else if (ntk.is_ci(fin_inp) == true && fin_inp != nd)
                                {
                                    if(fo_node)
                                    {
                                        if (!already_painted)
                                        {
                                            if (inv_flag)
                                            {
                                                /*Color first fan-out node south*/
                                                auto       color    = ctn.color_south;
                                                const auto finc_inv = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_inv.fanin_edges.cbegin(),
                                                              finc_inv.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { paint_edge_if(ctn, fe, color); });
                                                paint_if(ctn, new_output_node[i], color);

                                                /*Color other fan-out south*/
                                                color                  = ctn.color_south;
                                                const auto finc_no_inv = fanin_edges(ctn.color_ntk, new_output_node[i + 1]);
                                                std::for_each(finc_no_inv.fanin_edges.cbegin(),
                                                              finc_no_inv.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { paint_edge_if(ctn, fe, color); });
                                                paint_if(ctn, new_output_node[i+1], color);
                                            }
                                            else
                                            {
                                                /*Color first fan-out node south*/
                                                auto       color           = ctn.color_south;
                                                const auto finc_no_inv_one = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_no_inv_one.fanin_edges.cbegin(),
                                                              finc_no_inv_one.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { paint_edge_if(ctn, fe, color); });
                                                paint_if(ctn, new_output_node[i], color);

                                                i = 1 - i;

                                                /*Color other fan-out east*/
                                                color = ctn.color_east;
                                                const auto finc_no_inv_two =
                                                    fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_no_inv_two.fanin_edges.cbegin(),
                                                              finc_no_inv_two.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { paint_edge_if(ctn, fe, color); });
                                                paint_if(ctn, new_output_node[i], color);
                                            }
                                            already_painted = true;
                                        }
                                    }
                                    else
                                    {
                                        auto color = ctn.color_east;
                                        const auto finc_only_pis = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                        std::for_each(finc_only_pis.fanin_edges.cbegin(), finc_only_pis.fanin_edges.cend(),
                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                        paint_if(ctn, new_output_node[i], color);
                                    }
                                    node_pi = true;
                                    all_inputs = true;
                                }
                                /*else
                                {
                                    auto color = ctn.color_east;
                                    const auto finc_only_pis = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                    std::for_each(finc_only_pis.fanin_edges.cbegin(), finc_only_pis.fanin_edges.cend(),
                                                  [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                    paint_if(ctn, new_output_node[i], color);
                                }*/


                            });
                    }
                });
        });

#if (PROGRESS_BARS)
    // initialize a progress bar
    mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.num_gates()),
                                 "[i] determining relative positions: |{0}|"};
#endif

    rtv.foreach_gate(
        [&](const auto& n, [[maybe_unused]] const auto i)
        {
            const auto finc = fanin_edges(ctn.color_ntk, n);

            // if any incoming edge is colored east, color them all east, and south otherwise
            const auto color =
                std::any_of(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                            [&ctn](const auto& fe) { return ctn.color_ntk.edge_color(fe) == ctn.color_east; }) ?
                    ctn.color_east :
                    ctn.color_south;

            std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                          [&ctn, &color](const auto& fe) { recursively_paint_edges(ctn, fe, color); });

            // if all incoming edges are colored east, paint the node east as well
            if (std::all_of(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                            [&ctn](const auto& fe) { return ctn.color_ntk.edge_color(fe) == ctn.color_east; }))
            {
                ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, ctn.color_east);
            }
            // else, if all incoming edges are colored south, paint the node south as well
            else if (std::all_of(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                 [&ctn](const auto& fe) { return ctn.color_ntk.edge_color(fe) == ctn.color_south; }))
            {
                ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, ctn.color_south);
            }

#if (PROGRESS_BARS)
            // update progress
            bar(i);
#endif
        });

    return ctn;
}


template <typename Lyt, typename Ntk>
class orthogonal_new_impl
{
  public:
    orthogonal_new_impl(const Ntk& src, const orthogonal_physical_design_params& p, orthogonal_physical_design_stats& st) :
            ntk{topo_view_input_sort{mockturtle::fanout_view{inverter_balancing(fanout_substitution<mockturtle::names_view<technology_network>>(src))}}},
            ps{p},
            pst{st}
    {}

    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        // compute a coloring
        const auto ctn = new_east_south_edge_coloring(ntk);

        mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)> node2pos{ctn.color_ntk};

        // instantiate the layout
        //Lyt layout{determine_layout_size<Lyt>(ctn), twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

        Lyt layout{{1, 1, 1}, twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

        // reserve PI nodes without positions
        auto pi2node = reserve_input_nodes(layout, ctn.color_ntk);

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        tile<Lyt> latest_pos{1, 0};
        if(ctn.color_ntk.isFo_inv_flag()){
            ++latest_pos.x;
        }
        tile<Lyt> latest_pos_inputs{0, 0};

        std::cout<<"Inverter_Flag"<<ctn.color_ntk.isFo_inv_flag()<<std::endl;


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
                    // if node is a PI, move it to its correct position
                    if (ctn.color_ntk.is_pi(n))
                    {
                        /**NEW CODE
                         * !!new latest_pos for inputs needed
                         * **/
                        /*Placing Inputs in first Column*/
                        node2pos[n] = layout.move_node(pi2node[n], {latest_pos_inputs});
                        std::cout<<n<<"Pi plaziert auf"<<"X:"<<latest_pos_inputs.x<<"Y:"<<latest_pos_inputs.y<<std::endl;

                        //++latest_pos.y;
                        ++latest_pos_inputs.y;
                    }
                    /**NEW CODE MAJORITY GATES PLACEMENT**/
                    else if(ctn.color_ntk.is_maj(n))
                    {
                        const auto fc = fanins(ctn.color_ntk, n);
                        const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1], pre3 = fc.fanin_nodes[2];

                        auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                             pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]),
                             pre3_t = static_cast<tile<Lyt>>(node2pos[pre3]);

                        tile<Lyt>  t{}, t1{}, t2{}, t3{};

                        //pre1_t is northern tile
                        //pre2_t is middle tile
                        //pre3_t is southern tile
                        if (pre2_t.y < pre1_t.y)
                            std::swap(pre1_t, pre2_t);
                        if (pre3_t.y < pre1_t.y)
                            std::swap(pre1_t, pre3_t);
                        if (pre3_t.y < pre2_t.y)
                            std::swap(pre2_t, pre3_t);

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
                        /*if(pre2_t.y - pre3_t.y == 1)
                        {
                            //pre2_t gives y coordinates
                            t1 = {latest_pos.x + 4, pre2_t.y - 1};
                            t2 = {latest_pos.x + 3, pre2_t.y    };
                            t3 = {latest_pos.x    , pre2_t.y + 2};
                        }*/
                        if(pre3_t.y - pre1_t.y <= 3)
                        {
                            //pre1_t gives y coordinates
                            t1 = {latest_pos.x + 3, pre1_t.y    };
                            t2 = {latest_pos.x + 2, pre1_t.y + 1};
                            t3 = {latest_pos.x    , pre1_t.y + 3};
                        }
                        else
                        {
                            //pre3_t gives y coordinates
                            t1 = {latest_pos.x + 3, pre3_t.y - 3};
                            t2 = {latest_pos.x + 2, pre3_t.y - 2};
                            t3 = {latest_pos.x    , pre3_t.y    };
                        }
                        // coordinates of the majority gate
                        t = {t2.x, t2.y + 3};

                        //wire east
                        pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t1.x + 1, pre1_t.y}));
                        pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {t2.x + 1, pre2_t.y}));
                        pre3_t = static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {t3.x + 1, pre3_t.y}));

                        //Delay Network
                        //T1
                        pre1_t = static_cast<tile<Lyt>>(wire_south(layout, pre1_t, {pre1_t.x, t1.y + 5}));
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
                        pre3_t = static_cast<tile<Lyt>>(wire_south(layout, pre3_t, {pre3_t.x, t3.y + 3}));
                        pre3_t = static_cast<tile<Lyt>>(wire_east(layout, pre3_t, {pre3_t.x + 2, pre3_t.y}));
                        pre_clock = layout.get_clock_number({pre3_t});
                        pre3_t = static_cast<tile<Lyt>>(wire_north(layout, pre3_t, {pre3_t.x, pre3_t.y - 2}));
                        layout.assign_clock_number({pre3_t.x,pre3_t.y,0}, pre_clock+1);
                        pre_clock = layout.get_clock_number({pre3_t});
                        //end

                        node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre2_t, pre3_t, pre1_t);
                        std::cout<<n<<"Maj plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                        layout.assign_clock_number({t.x,t.y,0}, pre_clock+1);

                        pre_clock = layout.get_clock_number({t});
                        t = static_cast<tile<Lyt>>(wire_south(layout, t, {t.x, t.y + 3}));
                        layout.assign_clock_number({t.x,t.y-1,0}, pre_clock+1);
                        layout.assign_clock_number({t.x,t.y,0}, pre_clock+2);
                        pre_clock = layout.get_clock_number({t});
                        t = static_cast<tile<Lyt>>(wire_east(layout, t, {t.x +2, t.y}));
                        layout.assign_clock_number({t.x,t.y,0}, pre_clock+1);
                        t = static_cast<tile<Lyt>>(wire_north(layout, t, {t.x, t.y -2}));

                        latest_pos.x = t.x + 1;
                        latest_pos.y = t.y + 2;

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
                            if(ntk.isFo_inv_flag() && ntk.is_inv(n) && ntk.is_pi(pre)){
                                insert_position = 1;
                                --latest_pos.x;
                                std::cout<<"INV";
                            }
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

                        //insert buffer when a fan-out has two same colored outputs
                        if(ctn.color_ntk.is_fanout(pre1))
                        {
                            auto fos = fanouts(ctn.color_ntk, pre1);
                            if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }))
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                            }
                        }
                        if(ctn.color_ntk.is_fanout(pre2))
                        {
                            auto fos = fanouts(ctn.color_ntk, pre2);
                            if(std::all_of(fos.cbegin(), fos.cend(),
                                            [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }))
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                            }
                        }

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            // make sure pre1_t is the northwards tile
                            if (pre2_t.y < pre1_t.y)
                                std::swap(pre1_t, pre2_t);

                            // use larger y position of predecessors
                            t = {latest_pos.x, pre2_t.y};

                            // each 2-input gate has one incoming bent wire
                            pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));
                            std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                            std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                            std::cout<<n<<"color: "<<"east"<<std::endl;

                            ++latest_pos.x;
                            if(latest_pos.y < latest_pos_inputs.y){
                                latest_pos.y = t.y+1;
                            }
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            auto pre_fo = pre2;
                            // pre2_t is the westwards tile
                            if (pre2_t.x > pre1_t.x){
                                std::swap(pre1_t, pre2_t);
                                pre_fo = pre1;
                            }

                            /**NEW CODE
                             * !!new south wire option
                             * **/
                            // check if pre1_t is now also the northwards tile
                            if (pre1_t.y < pre2_t.y && !ctn.color_ntk.is_fanout(pre_fo))
                            {
                                if(pre2_t.x == pre1_t.x)
                                {
                                    // node can be placed on y position of pre2_t
                                    std::cout<<"Neue south wires für: "<<n<<std::endl;

                                    // use larger x position of predecessors
                                    t = {latest_pos.x, pre2_t.y};
                                    std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;
                                    ++latest_pos.x;
                                }
                                else
                                {
                                    // node can be placed on y position of pre2_t
                                    std::cout<<"Neue south wires für: "<<n<<std::endl;

                                    // use larger x position of predecessors
                                    t = {pre1_t.x, pre2_t.y};
                                    std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;
                                }
                                if(pre2_t.y+1>latest_pos.y)
                                {
                                    latest_pos.y =pre2_t.y+1;
                                }
                            }
                            else
                            {
                                if(latest_pos.y<latest_pos_inputs.y)
                                {
                                    // use larger x position of predecessors
                                    t = {pre1_t.x, latest_pos_inputs.y};
                                    std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;
                                    // each 2-input gate has one incoming bent wire
                                    pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                                    latest_pos.y = t.y+1;
                                }
                                else
                                {
                                    // use larger x position of predecessors
                                    t = {pre1_t.x, latest_pos.y};
                                    std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;
                                    // each 2-input gate has one incoming bent wire
                                    pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                                    ++latest_pos.y;
                                }
                            }


                        }
                        // n is colored null; corner case
                        else
                        {
                            // make sure pre1_t has an empty tile to its east and pre2_t to its south
                            if (!layout.is_empty_tile(layout.east(pre1_t)) ||
                                !layout.is_empty_tile(layout.south(pre2_t)))
                                std::swap(pre1_t, pre2_t);

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


                    // create PO at applicable position
                    if (ctn.color_ntk.is_po(n))
                    {
                        auto n_s = node2pos[n];

                        tile<Lyt> po_tile{};

                        // determine PO orientation
                        if (is_eastern_po_orientation_available(ctn, n))
                        {
                            ++latest_pos.x;
                            layout.resize({latest_pos.x-1, latest_pos.y-1, 1});
                            po_tile = layout.east(static_cast<tile<Lyt>>(n_s));
                        }
                        else
                        {
                            ++latest_pos.y;
                            layout.resize({latest_pos.x-1, latest_pos.y-1, 1});
                            po_tile = layout.south(static_cast<tile<Lyt>>(n_s));
                        }
                        if(ctn.color_ntk.is_maj(n))
                        {
                            ++po_tile.y;
                            n_s = static_cast<mockturtle::signal<Lyt>>(po_tile);
                            ++po_tile.x;
                        }

                        // check if PO position is located at the border
                        if (layout.is_at_eastern_border(po_tile))
                        {
                            layout.create_po(n_s,
                                             ctn.color_ntk.has_output_name(po_counter) ?
                                                 ctn.color_ntk.get_output_name(po_counter++) :
                                                 fmt::format("po{}", po_counter++),
                                             po_tile);
                        }
                        // place PO at the border and connect it by wire segments
                        else
                        {
                            const auto anker = layout.create_buf(n_s, po_tile);

                            po_tile = layout.eastern_border_of(po_tile);

                            layout.create_po(wire_east(layout, static_cast<tile<Lyt>>(anker), po_tile),
                                             ctn.color_ntk.has_output_name(po_counter) ?
                                                 ctn.color_ntk.get_output_name(po_counter++) :
                                                 fmt::format("po{}", po_counter++),
                                             po_tile);
                        }
                    }
                }

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
            });

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
    topo_view_input_sort<mockturtle::fanout_view<mockturtle::names_view<technology_network>>> ntk;

    orthogonal_physical_design_params ps;
    orthogonal_physical_design_stats& pst;

    uint32_t po_counter{0};
};

}  // namespace detail


template <typename Lyt, typename Ntk>
Lyt orthogonal_new(const Ntk& ntk, orthogonal_physical_design_params ps = {},
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
    detail::orthogonal_new_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_ORTHO_NEW_HPP