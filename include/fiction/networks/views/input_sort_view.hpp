//
// Created by Hien Benjamin on 02.06.2022.
//

#ifndef FICTION_INPUT_SORT_VIEW_HPP
#define FICTION_INPUT_SORT_VIEW_HPP

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/immutable_view.hpp>
#include <mockturtle/views/topo_view.hpp>
#include "mockturtle/networks/aig.hpp"
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

namespace fiction{

/*template <class TopoSorted, bool sorted = mockturtle::is_topologically_sorted_v<TopoSorted>>
class input_sort_view
{};*/

template<typename TopoSorted>
class input_sort_view : public TopoSorted{
  public:
    using storage = typename TopoSorted::storage;
    using node = typename TopoSorted::node;
    using signal = typename TopoSorted::signal;

    explicit input_sort_view(TopoSorted const& ts) : TopoSorted(ts)
    {
        static_assert(mockturtle::is_network_type_v<TopoSorted>, "Ntk is not a network type");
        static_assert(mockturtle::has_size_v<TopoSorted>, "Ntk does not implement the size function");
        static_assert(mockturtle::has_get_constant_v<TopoSorted>, "Ntk does not implement the get_constant function");
        static_assert(mockturtle::has_foreach_pi_v<TopoSorted>, "Ntk does not implement the foreach_pi function");
        static_assert(mockturtle::has_foreach_po_v<TopoSorted>, "Ntk does not implement the foreach_po function");
        static_assert(mockturtle::has_foreach_fanin_v<TopoSorted>, "Ntk does not implement the foreach_fanin function");
        static_assert(mockturtle::has_incr_trav_id_v<TopoSorted>, "Ntk does not implement the incr_trav_id function");
        static_assert(mockturtle::has_set_visited_v<TopoSorted>, "Ntk does not implement the set_visited function");
        static_assert(mockturtle::has_trav_id_v<TopoSorted>, "Ntk does not implement the trav_id function");
        static_assert(mockturtle::has_visited_v<TopoSorted>, "Ntk does not implement the visited function");

        copy_and_sort(ts);
    }

    template<typename Fn>
    void foreach_node( Fn&& fn ) const
    {
        mockturtle::detail::foreach_element( topo_order_input_sort.begin(), topo_order_input_sort.end(), fn);
        /*std::cout<<"IwAg";
        std::cout<<std::endl;*/
    }

    template<typename Fn>
    void foreach_pi( Fn&& fn ) const
    {
        mockturtle::detail::foreach_element( topo_order_input_sort.begin()+num_c, topo_order_input_sort.begin()+num_c+num_p, fn );
    }

    template <typename Ntk>
    void copy_and_sort(Ntk& ntk)
    {
        topo_order_input_sort.reserve(ntk.size());
        num_p = ntk.num_pis();
        mockturtle::fanout_view fov{ntk};
        //ntk.foreach_node([&](const auto& nd){topo_order_input_sort.push_back(nd);});

        bool pushed_pis = false;
        std::vector<node> wait;
        ntk.foreach_node([&](const auto& nd)
                         {
                             if(ntk.is_constant(nd))
                             {
                                 topo_order_input_sort.push_back(nd);
                                 ++num_c;
                             }
                             else if(ntk.is_ci(nd)){
                                 if(fov.fanout_size(nd)==2)
                                 {
                                     topo_order_input_sort.push_back(nd);
                                 }
                                 else {
                                     wait.push_back(nd);
                                 }
                             }
                             else{
                                 if(!pushed_pis){
                                     for(unsigned int iter = 0; iter < wait.size(); ++iter){
                                         topo_order_input_sort.push_back(wait[iter]);
                                     }
                                     pushed_pis = true;
                                 }
                                 topo_order_input_sort.push_back(nd);
                             }
                         });
    }

    const std::vector<node>& getTopo_order_input_sort() const
    {
        return topo_order_input_sort;
    }

  protected:
    std::vector<node> topo_order_input_sort;
    uint32_t num_p;
    uint32_t num_c = 0u;
};

}
#endif  // FICTION_INPUT_SORT_VIEW_HPP
