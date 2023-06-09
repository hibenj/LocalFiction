//
// Created by Hien Benjamin on 30.05.2022.
//

#ifndef FICTION_TOPO_VIEW_INPUT_SORT_HPP
#define FICTION_TOPO_VIEW_INPUT_SORT_HPP

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/immutable_view.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include "fiction/utils/network_utils.hpp"


#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>
#include <iostream>

namespace fiction
{

/**
 * A variation of mockturtle::topo_view that computes the topological order after sorting the inputs according to their fanout nodes.
 * This is part of the Distribution Newtork I: Inputs
 *
 * @tparam Ntk mockturtle network type.
 * @tparam sorted Flag that determines whether Ntk is already wrapped in a topo_view.
 */
template <class Ntk, bool sorted = mockturtle::is_topologically_sorted_v<Ntk>>
class topo_view_input_sort
{};

template <typename Ntk>
class topo_view_input_sort<Ntk, false> : public mockturtle::immutable_view<Ntk>
{
  public:
    using storage = typename Ntk::storage;
    using node    = typename Ntk::node;
    using signal  = typename Ntk::signal;

    /*! \brief Default constructor.
     *
     * Constructs topological view on another network.
     */
    explicit topo_view_input_sort(Ntk const& ntk) : mockturtle::immutable_view<Ntk>(ntk), num_p{ntk.num_pis()}, my_ntk{ntk}
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

        if constexpr (mockturtle::has_num_registers_v<Ntk>){
            num_r = ntk.num_registers();
        }

        update_topo();
    }

    /*! \brief Default constructor.
     *
     * Constructs topological view, but only for the transitive fan-in starting
     * from a given start signal.
     */
    topo_view_input_sort(Ntk const& ntk, typename Ntk::signal const& strt_sgnl) :
            mockturtle::immutable_view<Ntk>(ntk),
            start_signal(strt_sgnl), num_p{ntk.num_pis()}, my_ntk{ntk}
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

        update_topo();
    }

    /**
     * Reimplementation of `size`.
     */
    [[nodiscard]] auto size() const
    {
        return static_cast<uint32_t>(topo_order.size());
    }

    /**
     * Reimplementation of `num_gates`.
     */
    [[nodiscard]] auto num_gates() const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        return static_cast<uint32_t>(topo_order.size() - offset);
    }

    /**
     * Reimplementation of `node_to_index`.
     */
    [[nodiscard]] uint32_t node_to_index(node const& n) const
    {
        return static_cast<uint32_t>(
            std::distance(std::begin(topo_order), std::find(std::begin(topo_order), std::end(topo_order), n)));
    }

    /**
     * Reimplementation of `index_to_node`.
     */
    [[nodiscard]] node index_to_node(uint32_t index) const
    {
        return topo_order.at(index);
    }

    /**
     * Reimplementation of `foreach_node`.
     */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.begin(), topo_order.end(), fn);
    }

    /**
     * Reimplementation of `foreach_gate`.
     */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.begin(), topo_order.end() - offset, fn);
    }
    /*! \brief Reimplementation of `foreach_pi`. */
    template<typename Fn>
    void foreach_pi( Fn&& fn ) const
    {
        mockturtle::detail::foreach_element_if( topo_order.begin()+num_c, topo_order.begin()+num_c+num_p+num_r, [this]( auto n ) { return my_ntk.is_pi( n );}, fn );
    }

    /*! \brief Reimplementation of `foreach_ro`. */
    template<typename Fn>
    void foreach_ro( Fn&& fn ) const
    {
        mockturtle::detail::foreach_element_if( topo_order.begin()+num_c, topo_order.begin()+num_c+num_p+num_r, [this]( auto n ) { return my_ntk.is_ro( n );}, fn );
    }

    node ro_at(uint32_t index) const
    {
        std::vector<node> ROs;
        foreach_ro( [&](const auto& n){ROs.emplace_back(n);});
        return ROs[index];
    }

    /**
     * Reimplementation of `foreach_po`.
     *
     * If `start_signal` is provided in constructor, only this is returned as primary output, otherwise reverts to
     * original `foreach_po` implementation.
     */
    template <typename Fn>
    void foreach_po(Fn&& fn) const
    {
        if (start_signal)
        {
            std::vector<signal> signals(1, *start_signal);
            mockturtle::detail::foreach_element(signals.begin(), signals.end(), fn);
        }
        else
        {
            Ntk::foreach_po(fn);
        }
    }

    [[nodiscard]] uint32_t num_pos() const
    {
        return start_signal ? 1 : Ntk::num_pos();
    }

    [[nodiscard]] bool isFo_inv_flag() const
    {
        return fo_inv_flag;
    }

    [[nodiscard]] int isFo_inv_flag_num() const
    {
        return fo_inv_flag_num;
    }

    void update_topo()
    {
        topo_order.clear();

        this->incr_trav_id();
        this->incr_trav_id();
        topo_order.reserve(this->size());

        /* constants and PIs */
        const auto c0 = this->get_node(this->get_constant(false));
        topo_order.push_back(c0);
        ++num_c;
        this->set_visited(c0, this->trav_id());

        if (const auto c1 = this->get_node(this->get_constant(true)); this->visited(c1) != this->trav_id())
        {
            topo_order.push_back(c1);
            this->set_visited(c1, this->trav_id());
            ++num_c;
        }

        //this->foreach_node([this](auto n){if(my_ntk.is_constant(n))++num_c;});

        this->foreach_ci(
            [this](auto n)
            {
                if (this->visited(n) != this->trav_id())
                {input_sort(n);}
                });

        for(unsigned int iter = 0; iter < wait.size(); ++iter){
            topo_order.push_back(wait[iter]);
            this->set_visited(wait[iter], this->trav_id());
        }
        for(unsigned int iter = 0; iter < second_wait.size(); ++iter){
            topo_order.push_back(second_wait[iter]);
            this->set_visited(second_wait[iter], this->trav_id());
        }
        for(unsigned int iter = 0; iter < third_wait.size(); ++iter){
            topo_order.push_back(third_wait[iter]);
            this->set_visited(third_wait[iter], this->trav_id());
        }

        this->foreach_ci(
            [this](auto n)
            {
                if (this->visited(n) != this->trav_id())
                {
                    topo_order.push_back(n);
                    this->set_visited(n, this->trav_id());
                }
            });

        if (start_signal)
         {
             if (this->visited(this->get_node(*start_signal)) == this->trav_id())
                 return;
             create_topo_rec(this->get_node(*start_signal));
         }
         else
         {

             /*this->incr_trav_id();
             uint32_t reset_trav_id = this->trav_id();
             this->incr_trav_id();
             mockturtle::detail::foreach_element( topo_order.begin()+num_c, topo_order.begin()+num_c+num_p+num_r,
                                                 [this](auto is){topo_wait.push_back(is);});
             push_iter = 0;
             create_topo_rec(topo_wait[push_iter], reset_trav_id);*/

             Ntk::foreach_co( [this]( auto f ) {
                                 /* node was already visited */
                                 if ( this->visited( this->get_node( f ) ) == this->trav_id() )
                                     return;

                                 create_topo_rec( this->get_node( f ) );
                             } );
         }
    }



  private:
    void create_topo_rec(node const& n)
    {
        /* is permanently marked? */
        if (this->visited(n) == this->trav_id())
            return;

        /* ensure that the node is not temporarily marked */
        assert(this->visited(n) != this->trav_id() - 1);

        /* mark node temporarily */
        this->set_visited(n, this->trav_id() - 1);

        /* mark children */
        this->foreach_fanin(n, [this](signal const& f) { create_topo_rec(this->get_node(f)); });

        /* mark node n permanently */
        this->set_visited(n, this->trav_id());

        /* visit node */
        topo_order.push_back(n);
    }

    void create_rest_topo(node const& n, uint32_t tid)
    {
        ++overflow_protector;
        bool my_bool = false;

        const auto& fe = fanins(my_ntk, n);
        my_bool = std::all_of(fe.fanin_nodes.begin(), fe.fanin_nodes.end(), [&](const auto& fin_inp)
                                    {return this->visited(fin_inp) == this->trav_id();});
        bool is_seq = false;
        if constexpr (mockturtle::has_is_ro_v<Ntk>){
            if(my_ntk.is_ro(n)){
                is_seq = true;
            }
        }
        if(my_bool){
            if(!my_ntk.is_pi(n) & !is_seq){
                topo_order.push_back(n);
            }

            this->set_visited(n, this->trav_id());

            /*Push the outputs of a finally pushed node to the queue*/
            my_ntk.foreach_fanout(n, [&](auto fo){
                                   if(this->visited(fo) != tid){
                                       topo_wait.push_back(fo);
                                   }
                                   this->set_visited(fo, tid);
                               });
            topo_wait.erase(topo_wait.begin()+push_iter);
            push_iter=0;
        }
        else
        {
            ++push_iter;
        }

        if(!topo_wait.empty() && overflow_protector != 1000){
            create_rest_topo(topo_wait[push_iter], tid);
        }
    }

    void get_fo_network(node const& n)
    {
        my_ntk.foreach_fanout(n,
                        [&](const auto& fon) {
                                  node high_fanout = fon;
                            if (my_ntk.is_inv(fon))
                            {
                                auto fos = fanouts(my_ntk, fon);
                                high_fanout = fos[0];
                            }
                            if (my_ntk.is_fanout(fon))
                            {
                                high_fan_out_node.push_back(fon);
                                get_fo_network(high_fanout);
                            }
                        });
}

    void input_sort(node const& n){

        /*Place High Fan-out nodes first*/
        my_ntk.foreach_fanout(n, [&](const auto& fon)
                                  {
                                      bool is_fan_out = false;
                                      /*Ignore Inverters*/
                                      output_node.clear();
                                      output_node.push_back(fon);
                                      if (my_ntk.is_inv(fon))
                                      {
                                          /*Inverter Flag*/
                                          fo_inv_flag=true;
                                          ++fo_inv_flag_num;
                                          my_ntk.foreach_fanout(fon,
                                                             [&](const auto& fon_inv) { output_node[0] = fon_inv; });
                                      }
                                      high_fan_out_node.clear();
                                      high_fan_out_node.push_back(output_node[0]);
                                      /*get_fo_network(output_node[0]);
                                      for(auto i = high_fan_out_node.begin(); i<high_fan_out_node.end(); ++i){

                                      }*/

                                      if (my_ntk.is_fanout(output_node[0])){
                                          is_fan_out = true;
                                          /**New ordering don't just push FOs back**/
                                          /**Only push back related PIs**/
                                          /*topo_order.push_back(n);
                                          this->set_visited(n, this->trav_id());*/
                                          node safe_node = output_node[0];
                                          output_node.clear();
                                          my_ntk.foreach_fanout(safe_node,
                                                             [&](const auto& fon) {
                                                                    if (my_ntk.is_fanout(fon))
                                                                    {
                                                                        return;
                                                                    }
                                                                    else if (my_ntk.is_inv(fon))
                                                                    {
                                                                        /*Inverter Flag*/
                                                                        my_ntk.foreach_fanout(fon,
                                                                                              [&](const auto& fon_inv) { output_node.insert(output_node.begin(), fon_inv);});
                                                                     }
                                                                     else
                                                                         output_node.push_back(fon);
                                                             });
                                      }

                                      /*The Fan-out has to be connected to One PI or two PIs*/
                                      /*A PI has to e connected to another PI*/
                                      /**Ranking: FOs with 2PIs, FOs with 1PI, PI with PI, PIs connected to MAJ, Rest(visited in the main function)**/
                                      bool all_inputs = false;
                                      bool already_one_PI = false;
                                      node first_PI;
                                      for(int i = 0; i <output_node.size(); ++i)
                                      {
                                          my_ntk.foreach_fanin(
                                              output_node[i],
                                              [&](const auto& fi)
                                              {
                                                  auto fin_inp = my_ntk.get_node(fi);
                                                  /*Ignore Inverters*/
                                                  if (my_ntk.is_inv(fin_inp))
                                                  {
                                                      const auto fis_inv = fanins(my_ntk, fin_inp);
                                                      fin_inp            = fis_inv.fanin_nodes[0];
                                                  }
                                                  if (fin_inp != n)
                                                  {
                                                      /**Majority gates are ranked fourth**/
                                                      if (const auto fc = fanins(my_ntk, fon); my_ntk.is_maj(fon) && fc.fanin_nodes.size()>2)
                                                      {
                                                          if (this->visited(n) != this->trav_id())
                                                          {
                                                              third_wait.push_back(n);
                                                              this->set_visited(n, this->trav_id());
                                                          }
                                                      }
                                                      else if (my_ntk.is_ci(fin_inp))
                                                      {
                                                          if (is_fan_out)
                                                          {
                                                              /**When a Fanout is connected to a PI, it can be one or two PIs**/
                                                              if(already_one_PI)
                                                              {
                                                                  /**FO has two incoming PIs**/
                                                                  /**Push PI and PI stored in first_PI**/
                                                                  auto check_n = wait[wait.size()-2];
                                                                  wait.erase(wait.end()-2);
                                                                  if(check_n == n)
                                                                  {
                                                                      topo_order.push_back(n);
                                                                      this->set_visited(n, this->trav_id());
                                                                  }
                                                                  else
                                                                      assert(false);

                                                                  if (this->visited(fin_inp) != this->trav_id())
                                                                  {
                                                                      topo_order.push_back(fin_inp);
                                                                      this->set_visited(fin_inp, this->trav_id());
                                                                  }
                                                                  already_one_PI = false;

                                                              }
                                                              else
                                                              {
                                                                  /**FO has min one incoming PI**/
                                                                  /**Store PI in first_PI, for the case if FO has two incoming PIs**/
                                                                  if (this->visited(n) != this->trav_id())
                                                                  {
                                                                      wait.push_back(n);
                                                                      this->set_visited(n, this->trav_id());
                                                                  }
                                                                  if (this->visited(fin_inp) != this->trav_id())
                                                                  {
                                                                      wait.push_back(fin_inp);
                                                                      this->set_visited(fin_inp, this->trav_id());
                                                                  }
                                                                  already_one_PI = true;
                                                                  first_PI       = fin_inp;
                                                              }
                                                          }
                                                          else
                                                          {
                                                              /**When a PI is connected to a PI it is ranked third**/
                                                              if (this->visited(n) != this->trav_id())
                                                              {
                                                                  second_wait.push_back(n);
                                                                  this->set_visited(n, this->trav_id());
                                                              }
                                                              if (this->visited(fin_inp) != this->trav_id())
                                                              {
                                                                  second_wait.push_back(fin_inp);
                                                                  this->set_visited(fin_inp, this->trav_id());
                                                              }
                                                          }
                                                      }
                                                  }
                                              });
                                      }
                           });


    }

  private:
    Ntk my_ntk;
    std::vector<node>     topo_order;
    std::optional<signal> start_signal;

    std::vector<node> topo_wait;
    std::vector<node> wait;
    std::vector<node> second_wait;
    std::vector<node> third_wait;
    std::vector<node> output_node;
    std::vector<node> high_fan_out_node;


    bool fo_inv_flag = false;
    int fo_inv_flag_num = 0;

    uint32_t num_p;
    uint32_t num_c = 0u;
    uint32_t num_r = 0u;

    uint32_t overflow_protector{0};

    unsigned int push_iter;
    node next_node;
};

template <typename Ntk>
class topo_view_input_sort<Ntk, true> : public Ntk
{
  public:
    explicit topo_view_input_sort(Ntk const& ntk) : Ntk(ntk) {}
};

template <class T>
topo_view_input_sort(T const&) -> topo_view_input_sort<T>;

template <class T>
topo_view_input_sort(T const&, typename T::signal const&) -> topo_view_input_sort<T>;

}  // namespace fiction

#endif  // FICTION_TOPO_VIEW_INPUT_SORT_HPP
