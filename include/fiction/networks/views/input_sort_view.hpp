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

template<typename Ntk>
class input_sort_view : public Ntk{
  public:
    using storage = typename Ntk::storage;
    using node = typename Ntk::node;
    using signal = typename Ntk::signal;

    explicit input_sort_view(Ntk const& ntk) : Ntk(ntk), num_p{ntk.num_pis()}
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size function");
        static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant function");
        static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi function");
        static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po function");
        static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function");
        static_assert(mockturtle::has_incr_trav_id_v<Ntk>, "Ntk does not implement the incr_trav_id function");
        static_assert(mockturtle::has_set_visited_v<Ntk>, "Ntk does not implement the set_visited function");
        static_assert(mockturtle::has_trav_id_v<Ntk>, "Ntk does not implement the trav_id function");
        static_assert(mockturtle::has_visited_v<Ntk>, "Ntk does not implement the visited function");
        static_assert(mockturtle::has_foreach_fanout_v<Ntk>, "Ntk does not implement the has_foreach_fanout function");

        copy_and_sort(ntk);
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

    void copy_and_sort(Ntk const& ntk)
    {
        //std::cout << typeid(ntk).name() << std::endl;
        topo_order_input_sort.reserve(ntk.size());
        bool pushed_pis = false;
        int non_inv = 0;
        bool all_fanins_single_inputs = false;
        unsigned int single_input_iterator = 0;
        this->incr_trav_id();

        ntk.foreach_node([&](const auto& nd)
                         {
                             if(ntk.is_constant(nd))
                             {
                                 topo_order_input_sort.push_back(nd);
                                 this->set_visited(nd, this->trav_id());
                                 ++num_c;
                             }
                             else if(ntk.is_ci(nd))
                             {
                                 if (!this->visited(nd) == this->trav_id())
                                 {
                                     //std::cout<<"TravID: "<<this->trav_id()<<std::endl;
                                     /*Place connected input-nodes first*/
                                     ntk.foreach_fanout(
                                         nd,
                                         [&](const auto& fon)
                                         {
                                             /*Ignore Inverters*/
                                             output_node.clear();
                                             output_node.push_back(fon);
                                             if (ntk.is_inv(fon))
                                             {
                                                 /*Inverter Flag*/
                                                 ntk.foreach_fanout(fon,
                                                                    [&](const auto& fon_inv) { output_node[0] = fon_inv; });
                                             }
                                             if (ntk.is_fanout(output_node[0])){
                                                 node safe_node = output_node[0];
                                                 output_node.clear();
                                                 ntk.foreach_fanout(safe_node,
                                                                    [&](const auto& fon) {
                                                                        if (ntk.is_inv(fon))
                                                                        {
                                                                            /*Inverter Flag*/
                                                                            ntk.foreach_fanout(fon,
                                                                                               [&](const auto& fon_inv) { output_node.push_back(fon_inv); });
                                                                        }else
                                                                            output_node.insert(output_node.begin(), fon);
                                                                    });
                                             }

                                             /*The Fan-out has to be connected to 1.Fan-out nodes of Inputs or 2.Inputs*/
                                             /*Has to be rewritten for Inputs with Fan-outs higher than two*/
                                             bool all_inputs = false;
                                             for(int i = 0; i <output_node.size(); ++i)
                                             {
                                                 ntk.foreach_fanin(
                                                     output_node[i],
                                                     [&](const auto& fi)
                                                     {
                                                         auto fin_inp = ntk.get_node(fi);
                                                         /*Ignore Inverters*/
                                                         if (ntk.is_inv(fin_inp))
                                                         {
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
                                                                         all_inputs = true;
                                                                         std::cout
                                                                             << "Node: " << nd
                                                                             << std::endl;
                                                                         std::cout
                                                                             << "Connected Fan-out PI: " << fin_inp_sec
                                                                             << std::endl;
                                                                         if (!this->visited(nd) == this->trav_id()){
                                                                             wait_fo.insert(wait_fo.begin(), nd);
                                                                             this->set_visited(nd, this->trav_id());
                                                                         }

                                                                         if (!this->visited(fin_inp_sec) == this->trav_id()){
                                                                             wait_fo.insert(wait_fo.begin(), fin_inp_sec);
                                                                             this->set_visited(fin_inp_sec, this->trav_id());
                                                                         }

                                                                     }
                                                                 });
                                                         }
                                                         /*2*/
                                                         else if (ntk.is_ci(fin_inp) == true && fin_inp != nd)
                                                         {
                                                             std::cout
                                                                 << "Node: " << nd
                                                                 << std::endl;
                                                             std::cout << "Connected to normal PI: " << fin_inp
                                                                       << std::endl;
                                                             all_inputs = true;
                                                         }
                                                     });
                                             }
                                         });
                                 }
                             }
                             else
                             {
                                 if(!pushed_pis)
                                 {
                                     for(unsigned int iter = 0; iter < wait_fo.size(); ++iter){
                                         topo_order_input_sort.push_back(wait_fo[iter]);
                                     }
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
    std::vector<node> output_node;
    std::vector<node> wait_fo;
    std::vector<node> wait;
};

}
#endif  // FICTION_INPUT_SORT_VIEW_HPP
