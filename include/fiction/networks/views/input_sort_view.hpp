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
#include "mockturtle/views/names_view.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"


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

    explicit input_sort_view(TopoSorted const& ts) : TopoSorted(ts), num_p{ts.num_pis()}
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

    /*! \brief Reimplementation of `node_to_index`. */
    uint32_t node_to_index( node const& n ) const
    {
        return std::distance( std::begin( topo_order_input_sort ), std::find( std::begin( topo_order_input_sort ), std::end( topo_order_input_sort ), n ) );
    }

    /*! \brief Reimplementation of `index_to_node`. */
    node index_to_node( uint32_t index ) const
    {
        return topo_order_input_sort.at( index );
    }

    /*! \brief Reimplementation of `foreach_node`. */
    template<typename Fn>
    void foreach_node( Fn&& fn ) const
    {
        mockturtle::detail::foreach_element( topo_order_input_sort.begin(), topo_order_input_sort.end(), fn);
        /*std::cout<<"IwAg";
        std::cout<<std::endl;*/
    }

    template<typename Fn>
    void foreach_ci( Fn&& fn ) const
    {
        mockturtle::detail::foreach_element( topo_order_input_sort.begin()+num_c, topo_order_input_sort.begin()+num_c+num_p, fn );
    }

    /*! \brief Reimplementation of `foreach_pi`. */
    template<typename Fn>
    void foreach_pi( Fn&& fn ) const
    {
        mockturtle::detail::foreach_element( topo_order_input_sort.begin()+num_c, topo_order_input_sort.begin()+num_c+num_p, fn );
    }

    [[nodiscard]] bool isFo_one_inv_flag() const
    {
        return fo_one_inv_flag;
    }

    [[nodiscard]] bool isFo_two_inv_flag() const
    {
        return fo_two_inv_flag;
    }

    template <typename Ntk>
    void copy_and_sort(Ntk& ntk)
    {
        topo_order_input_sort.reserve(ntk.size());
        mockturtle::fanout_view fov{ntk};
        bool pushed_pis = false;
        std::vector<node> wait;
        int non_inv = 0;
        bool all_fanins_single_inputs = false;
        unsigned int single_input_iterator = 0;

        ntk.foreach_node([&](const auto& nd)
                         {
                             if(ntk.is_constant(nd))
                             {
                                 topo_order_input_sort.push_back(nd);
                                 ++num_c;
                             }
                             else if(ntk.is_ci(nd))
                             {
                                 /*Place High Fan-out nodes first*/
                                 if(fov.fanout_size(nd)>=2)
                                 {
                                     topo_order_input_sort.push_back(nd);
                                     fov.foreach_fanout(nd, [&](const auto& fo)
                                                        {
                                                            const auto fe = fanins(fov, fo);
                                                            all_fanins_single_inputs = std::all_of(fe.fanin_nodes.begin(), fe.fanin_nodes.begin(), [&](const auto& fin_inp)
                                                                                                   {
                                                                                                       if(fin_inp!=nd && fov.is_ci(fin_inp)==1){
                                                                                                           return fov.is_ci(fin_inp)== true;
                                                                                                       }
                                                                                                       else return fov.is_ci(fin_inp)== false;
                                                                                                   });
                                                        });
                                 }
                                 else
                                 {
                                     /*This is only one Fan-out*/
                                     fov.foreach_fanout(nd, [&](const auto& fo)
                                         {
                                                            /*All Fan-in nodes need to be inputs*/
                                                            bool all_inputs = true;
                                                            fov.foreach_fanin(fo, [&](const auto& fi){const auto fin_inp = ntk.get_node(fi); if(fov.is_ci(fin_inp)== false){all_inputs = false;}});
                                                            /*Place inputs first, which are connected to a high-fan-out input over one node*/
                                                            if(all_inputs)
                                                            {
                                                                fov.foreach_fanin(fo, [&](const auto& fi)
                                                                                  {
                                                                                      //12 34
                                                                                      if(const auto fin = ntk.get_node(fi); fin!=nd)
                                                                                      {
                                                                                          if (fov.fanout_size(fin) >= 2)
                                                                                          {
                                                                                              if (fov.is_complemented(fi))
                                                                                              {
                                                                                                  fo_two_inv_flag = true;
                                                                                                  if (!all_fanins_single_inputs)
                                                                                                  {
                                                                                                      fo_two_inv_flag = false;
                                                                                                  }
                                                                                                  wait.insert(wait.begin()+non_inv, nd);
                                                                                              }
                                                                                              else
                                                                                              {
                                                                                                  ++non_inv;
                                                                                                  wait.insert(wait.begin(), nd);
                                                                                              }

                                                                                          }
                                                                                          else
                                                                                          {
                                                                                              if(single_input_iterator==0)
                                                                                              {
                                                                                                  wait.push_back(nd);
                                                                                                  ++single_input_iterator;
                                                                                              }
                                                                                              else if(single_input_iterator>=1)
                                                                                              {
                                                                                                  if (!wait.empty())
                                                                                                  {
                                                                                                      const auto fo_node = fanouts(fov ,nd);
                                                                                                      const auto fo_wait = fanouts(fov, wait.at(wait.size()-single_input_iterator));
                                                                                                      std::cout<<"fo_node"<<fo_node[0]<<std::endl;

                                                                                                      std::cout<<"fo_wait"<<fo_wait[0]<<std::endl;
                                                                                                      if(fo_node[0]==fo_wait[0]){
                                                                                                          wait.insert(wait.end()-single_input_iterator+1, nd);
                                                                                                      }

                                                                                                      else
                                                                                                      {
                                                                                                          wait.push_back(nd);
                                                                                                      }
                                                                                                      ++single_input_iterator;
                                                                                                  }
                                                                                              }

                                                                                          }
                                                                                      }
                                                                                      else if (fov.is_complemented(fi) && fin==nd)
                                                                                      {
                                                                                          fo_one_inv_flag = true;
                                                                                      }
                                                                                  });
                                                            }
                                                            else
                                                            {
                                                                wait.push_back(nd);
                                                            }
                                     });
                                 }
                             }
                             else
                             {
                                 if(!pushed_pis)
                                 {
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
    bool fo_one_inv_flag = false;
    bool fo_two_inv_flag = false;
};

}
#endif  // FICTION_INPUT_SORT_VIEW_HPP
