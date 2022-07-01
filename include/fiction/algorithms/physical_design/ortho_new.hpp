//
// Created by Hien Benjamin on 24.05.2022.
//

#ifndef FICTION_ORTHO_NEW_HPP
#define FICTION_ORTHO_NEW_HPP

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
                        /*Inverter Flag*/
                        ntk.foreach_fanout(fon,
                                           [&](const auto& fon_inv) {
                                               new_output_node[0] = fon_inv;

                                               /*COLOR INVERTER EAST*/
                                               auto color = ctn.color_east;
                                               const auto finc = fanin_edges(ctn.color_ntk, fon);
                                               std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                                             [&ctn, &color](const auto& fe) { ctn.color_ntk.paint_edge(fe, color); });
                                           });
                    }
                    if (ntk.is_fanout(new_output_node[0])){
                        mockturtle::node<Ntk> safe_node = new_output_node[0];
                        fo_node = true;

                        /*COLOR FAN-OUT EAST*/
                        auto color = ctn.color_east;
                        const auto finc = fanin_edges(ctn.color_ntk, new_output_node[0]);
                        std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                      [&ctn, &color](const auto& fe) { ctn.color_ntk.paint_edge(fe, color); });

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
                                                  [&ctn, &color](const auto& fe) { ctn.color_ntk.paint_edge(fe, color); });
                                    ctn.color_ntk.paint(mockturtle::node<Ntk>{fin_inp}, color);
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
                                                    /*Color first fan-out node east*/
                                                    auto color = ctn.color_east;
                                                    const auto finc_fo = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                    std::for_each(finc_fo.fanin_edges.cbegin(), finc_fo.fanin_edges.cend(),
                                                                  [&ctn, &color](const auto& fe) { ctn.color_ntk.paint_edge(fe, color); });
                                                    ctn.color_ntk.paint(mockturtle::node<Ntk>{new_output_node[i]}, color);

                                                    if(i == 0){
                                                        ++i;
                                                    }else
                                                        --i;

                                                    /*Color other fan-out south*/
                                                    color = ctn.color_south;
                                                    const auto finc_other = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                    std::for_each(finc_other.fanin_edges.cbegin(), finc_other.fanin_edges.cend(),
                                                                  [&ctn, &color](const auto& fe) { ctn.color_ntk.paint_edge(fe, color); });
                                                    ctn.color_ntk.paint(mockturtle::node<Ntk>{new_output_node[i]}, color);

                                                    already_painted = true;
                                                }
                                                /*else if(!already_painted){
                                                    auto color = ctn.color_south;
                                                    const auto finc_other = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                    std::for_each(finc_other.fanin_edges.cbegin(), finc_other.fanin_edges.cend(),
                                                                  [&ctn, &color](const auto& fe) { ctn.color_ntk.paint_edge(fe, color); });
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
                                                /*Color first fan-out node east*/
                                                auto       color    = ctn.color_south;
                                                const auto finc_inv = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_inv.fanin_edges.cbegin(),
                                                              finc_inv.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { ctn.color_ntk.paint_edge(fe, color); });
                                                ctn.color_ntk.paint(mockturtle::node<Ntk>{new_output_node[i]}, color);

                                                /*Color other fan-out south*/
                                                color                  = ctn.color_south;
                                                const auto finc_no_inv = fanin_edges(ctn.color_ntk, new_output_node[i + 1]);
                                                std::for_each(finc_no_inv.fanin_edges.cbegin(),
                                                              finc_no_inv.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { ctn.color_ntk.paint_edge(fe, color); });
                                                ctn.color_ntk.paint(mockturtle::node<Ntk>{new_output_node[i+1]}, color);
                                            }
                                            else
                                            {
                                                /*Color first fan-out node south*/
                                                auto       color           = ctn.color_south;
                                                const auto finc_no_inv_one = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_no_inv_one.fanin_edges.cbegin(),
                                                              finc_no_inv_one.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { ctn.color_ntk.paint_edge(fe, color); });
                                                ctn.color_ntk.paint(mockturtle::node<Ntk>{new_output_node[i]}, color);

                                                if(i == 0){
                                                    ++i;
                                                }else
                                                    --i;

                                                /*Color other fan-out east*/
                                                color = ctn.color_east;
                                                const auto finc_no_inv_two =
                                                    fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_no_inv_two.fanin_edges.cbegin(),
                                                              finc_no_inv_two.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { ctn.color_ntk.paint_edge(fe, color); });
                                                ctn.color_ntk.paint(mockturtle::node<Ntk>{new_output_node[i]}, color);
                                            }
                                            already_painted = true;
                                        }
                                    }
                                    else
                                    {
                                        auto color = ctn.color_east;
                                        const auto finc_only_pis = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                        std::for_each(finc_only_pis.fanin_edges.cbegin(), finc_only_pis.fanin_edges.cend(),
                                                      [&ctn, &color](const auto& fe) { ctn.color_ntk.paint_edge(fe, color); });
                                        ctn.color_ntk.paint(mockturtle::node<Ntk>{new_output_node[i]}, color);
                                    }
                                    node_pi = true;
                                    all_inputs = true;
                                }


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

template <typename Lyt, typename Ntk, typename Ps>
aspect_ratio<Lyt> determine_new_layout_size(const Ntk &ntk, const Ps& ps) noexcept
{
    tile<Lyt> latest_pos{1, 0};
    if(ntk.isFo_inv_flag()){
        ++latest_pos.x;
    }
    tile<Lyt> latest_pos_inputs{0, 0};
    const auto ctn = new_east_south_edge_coloring(ntk);

    mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)> node2pos{ctn.color_ntk};

    Lyt layout{determine_layout_size<Lyt>(ctn), twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

    auto pi2node = reserve_input_nodes(layout, ctn.color_ntk);


    uint64_t x = 0ull, y = ctn.color_ntk.num_pis() - 1;


    ntk.foreach_node(
            [&](const auto& n, [[maybe_unused]] const auto i)
            {
        // do not place constants
        if (!ctn.color_ntk.is_constant(n))
        {
            // if node is a PI, move it to its correct position
            if (ctn.color_ntk.is_pi(n))
            {
                node2pos[n] = layout.move_node(pi2node[n], {latest_pos_inputs});

                ++latest_pos_inputs.y;
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
                if(ctn.color_ntk.is_fanout(n) && ctn.color_ntk.is_pi(pre))
                {
                    ++latest_pos.y;
                }
                const tile<Lyt> t{insert_position, pre_t.y};
                node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                ++latest_pos.x;
            }
            // n is colored south
            else if (clr == ctn.color_south)
            {
                if(ctn.color_ntk.is_inv(n) && latest_pos.y<latest_pos_inputs.y){
                    const tile<Lyt> t{pre_t.x, latest_pos_inputs.y};
                    node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                    ++latest_pos.y;
                }
                else
                {
                    const tile<Lyt> t{pre_t.x, latest_pos.y};
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

                ++latest_pos.x;
                if(latest_pos.y < latest_pos_inputs.y){
                    latest_pos.y = t.y+1;
                }
            }
            // n is colored south
            else if (clr == ctn.color_south)
            {
                // pre2_t is the westwards tile
                if (pre2_t.x > pre1_t.x)
                    std::swap(pre1_t, pre2_t);

                // check if pre1_t is now also the northwards tile
                if (pre1_t.y < pre2_t.y)
                {
                    if(pre2_t.x == pre1_t.x)
                    {

                        // use larger x position of predecessors
                        t = {latest_pos.x, pre2_t.y};
                        latest_pos.y =pre2_t.y+1;
                        ++latest_pos.x;
                    }
                    else
                    {
                        // node can be placed on y position of pre2_t

                        // use larger x position of predecessors
                        t = {pre1_t.x, pre2_t.y};
                        latest_pos.y =pre2_t.y+1;
                    }
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
            // n is colored null; corner case
            else
            {
                // make sure pre1_t has an empty tile to its east and pre2_t to its south
                if (!layout.is_empty_tile(layout.east(pre1_t)) ||
                    !layout.is_empty_tile(layout.south(pre2_t)))
                    std::swap(pre1_t, pre2_t);

                t = latest_pos;

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
            const auto n_s = node2pos[n];

            tile<Lyt> po_tile{};

            // determine PO orientation
            if (is_eastern_po_orientation_available(ctn, n))
            {
                po_tile = layout.east(static_cast<tile<Lyt>>(n_s));
                ++latest_pos.x;
            }
            else
            {
                po_tile = layout.south(static_cast<tile<Lyt>>(n_s));
                ++latest_pos.y;
            }
        }
                }
});

    return {latest_pos.x-1, latest_pos.y-1, 1};
}

template <typename Lyt, typename Ntk>
class orthogonal_new_impl
{
  public:
    orthogonal_new_impl(const Ntk& src, const orthogonal_physical_design_params& p, orthogonal_physical_design_stats& st) :
            ntk{topo_view_input_sort{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(src)}}},
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

        Lyt layout{determine_new_layout_size<Lyt>(ntk, ps), twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

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
                    // if n has only one fanin
                    else if (const auto fc = fanins(ctn.color_ntk, n); fc.fanin_nodes.size() == 1)
                    {
                        const auto& pre = fc.fanin_nodes[0];

                        const auto pre_t = static_cast<tile<Lyt>>(node2pos[pre]);

                        /**NEW CODE
                         * !!fan-outs from PIs are colored east
                         * **/
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

                            std::cout<<n<<"FO oder Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre"<<pre<<std::endl;
                            std::cout<<n<<"color: "<<"east"<<std::endl;
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            if(ctn.color_ntk.is_inv(n) && latest_pos.y<latest_pos_inputs.y){
                                const tile<Lyt> t{pre_t.x, latest_pos_inputs.y};
                                std::cout<<n<<"FO oder Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                std::cout<<n<<"Pre"<<pre<<std::endl;
                                std::cout<<n<<"color: "<<"south"<<std::endl;
                                node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                                ++latest_pos.y;
                            }
                            else
                            {
                                const tile<Lyt> t{pre_t.x, latest_pos.y};
                                std::cout<<n<<"FO oder Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
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
                            // pre2_t is the westwards tile
                            if (pre2_t.x > pre1_t.x)
                                std::swap(pre1_t, pre2_t);

                            /**NEW CODE
                             * !!new south wire option
                             * **/
                            // check if pre1_t is now also the northwards tile
                            if (pre1_t.y < pre2_t.y)
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
                                    latest_pos.y =pre2_t.y+1;
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
                                    latest_pos.y =pre2_t.y+1;
                                }
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
                        // n is colored null; corner case
                        else
                        {
                            // make sure pre1_t has an empty tile to its east and pre2_t to its south
                            if (!layout.is_empty_tile(layout.east(pre1_t)) ||
                                !layout.is_empty_tile(layout.south(pre2_t)))
                                std::swap(pre1_t, pre2_t);

                            t = latest_pos;

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
                        const auto n_s = node2pos[n];

                        tile<Lyt> po_tile{};

                        // determine PO orientation
                        if (is_eastern_po_orientation_available(ctn, n))
                        {
                            po_tile = layout.east(static_cast<tile<Lyt>>(n_s));
                            ++latest_pos.x;
                        }
                        else
                        {
                            po_tile = layout.south(static_cast<tile<Lyt>>(n_s));
                            ++latest_pos.y;
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
    if (has_high_degree_fanin_nodes(ntk, 2))
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