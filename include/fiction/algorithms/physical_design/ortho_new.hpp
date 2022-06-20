//
// Created by Hien Benjamin on 24.05.2022.
//

#ifndef FICTION_ORTHO_NEW_HPP
#define FICTION_ORTHO_NEW_HPP

#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/networks/views/topo_view_input_sort.hpp"

int create_ten(){
    int a = 10;
    return a;
};

namespace fiction
{
namespace detail
{

template <typename Lyt, typename Ntk>
class orthogonal_new_impl
{
  public:
    orthogonal_new_impl(const Ntk& src, const orthogonal_physical_design_params& p, orthogonal_physical_design_stats& st) :
            ntk{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(input_sort_view(src))}},
            ps{p},
            pst{st}
    {}

    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        // compute a coloring
        const auto ctn = east_south_edge_coloring(ntk);

        input_sort_view isv{ntk};

        mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)> node2pos{ctn.color_ntk};

        // instantiate the layout
        Lyt layout{determine_layout_size<Lyt>(ctn), twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

        // reserve PI nodes without positions
        auto pi2node = reserve_input_nodes(layout, ctn.color_ntk);

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        tile<Lyt> latest_pos{1, 0};
        tile<Lyt> latest_pos_inputs{0, 0};


#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.size()), "[i] arranging layout: |{0}|"};
#endif

        ctn.color_ntk.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] const auto i)
            {
                //std::cout<<"n:"<<n<<" ";
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
                        //std::cout<<"n: "<<n<<std::endl;
                        std::cout<<n<<"Pi plaziert auf"<<"X:"<<latest_pos_inputs.x<<"Y:"<<latest_pos_inputs.y<<std::endl;
                        /**NEW CODE
                         * !!commented out, conflicts need to be solved somewhere else
                         * **/
                        // resolve conflicting PIs
                        /*ctn.color_ntk.foreach_fanout(
                            n,
                            [&ctn, &n, &layout, &node2pos, &latest_pos](const auto& fon)
                            {
                                if (ctn.color_ntk.color(fon) == ctn.color_south)
                                {
                                    node2pos[n] =
                                        layout.create_buf(wire_east(layout, {0, latest_pos.y}, latest_pos), latest_pos);
                                    ++latest_pos.x;
                                }

                                // PIs have only one fanout
                                return false;
                            });*/
                        /**NEW CODE
                         * !!commented out
                         * **/
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
                        if(ctn.color_ntk.is_fanout(n) && ctn.color_ntk.is_pi(pre)){
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, ctn.color_east);
                            ++latest_pos.y;
                        }

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            const tile<Lyt> t{latest_pos.x, pre_t.y};
                            std::cout<<n<<"FO oder Inv plaziert auf"<<"X:"<<latest_pos.x<<"Y:"<<pre_t.y<<std::endl;
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            if(ctn.color_ntk.is_inv(n) && latest_pos.y<latest_pos_inputs.y){
                                const tile<Lyt> t{pre_t.x, latest_pos_inputs.y};
                                std::cout<<n<<"FO oder Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                                ++latest_pos.y;
                            }
                            else
                            {
                                const tile<Lyt> t{pre_t.x, latest_pos.y};
                                std::cout<<n<<"FO oder Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
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
                            std::cout<<n<<"And plaziert auf"<<"X:"<<t.x + 1<<"Y:"<<pre1_t.y<<std::endl;

                            ++latest_pos.x;
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
                                // node can be placed on y position of pre2_t
                                std::cout<<"Neue south wires für: "<<n<<std::endl;

                                // use larger x position of predecessors
                                t = {pre1_t.x, pre2_t.y};
                                std::cout<<n<<"And plaziert auf"<<"X:"<<pre1_t.x<<"Y:"<<pre2_t.y<<std::endl;
                                latest_pos.y =pre2_t.y+1;
                            }
                            else
                            {
                                // use larger x position of predecessors
                                t = {pre1_t.x, latest_pos.y};
                                std::cout<<n<<"And plaziert auf"<<"X:"<<pre1_t.x<<"Y:"<<latest_pos.y<<std::endl;
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

        //std::cout<<"latest X: "<<latest_pos.x<<std::endl;
        //std::cout<<"latest Y: "<<latest_pos.y<<std::endl;

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