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
 * A variation of mockturtle::topo_view that computes the reverse topological order. Unfortunately, this view could not
 * just extend mockturtle::topo_view and override its update_topo function because the necessary data members are
 * private instead of protected...
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
        mockturtle::detail::foreach_element( topo_order.begin()+num_c, topo_order.begin()+num_c+num_p, fn );
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

    [[nodiscard]] bool isFo_one_inv_flag() const
    {
        return fo_one_inv_flag;
    }

    [[nodiscard]] bool isFo_two_inv_flag() const
    {
        return fo_two_inv_flag;
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
        this->set_visited(c0, this->trav_id());

        if (const auto c1 = this->get_node(this->get_constant(true)); this->visited(c1) != this->trav_id())
        {
            topo_order.push_back(c1);
            this->set_visited(c1, this->trav_id());
        }

        this->foreach_node([this](auto n){if(my_ntk.is_constant(n))++num_c;});

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

        if (start_signal)
         {
             if (this->visited(this->get_node(*start_signal)) == this->trav_id())
                 return;
             create_topo_rec(this->get_node(*start_signal));
         }
         else
         {
             /*Ntk::foreach_co(
                 [this](auto f)
                 {
                      *//*node was already visited*//*
       if (this->visited(this->get_node(f)) == this->trav_id())
           return;

       create_topo_rec(this->get_node(f));
                });*/

             this->incr_trav_id();
             uint32_t reset_trav_id = this->trav_id();
             this->incr_trav_id();
             mockturtle::detail::foreach_element( topo_order.begin()+num_c, topo_order.begin()+num_c+num_p,
                                                 [this](auto is){topo_wait.push_back(is);});
             /*mockturtle::detail::foreach_element( topo_order.begin()+num_c, topo_order.begin()+num_c+num_p,
                                                 [this](auto is){fov.foreach_fanout(is, [this, &is](auto fo){
                                                    std::cout<<"Node:"<<is<<"Fanouts: "<<fo<<std::endl;
                                                    });
                                                 });*/
             push_iter = 0;
             create_rest_topo(topo_wait[push_iter], reset_trav_id);
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
        bool my_bool = false;

        const auto fe = fanins(my_ntk, n);
        my_bool = std::all_of(fe.fanin_nodes.begin(), fe.fanin_nodes.end(), [&](const auto& fin_inp)
                                    {return this->visited(fin_inp) == this->trav_id();});
        if(my_bool){
            if(!my_ntk.is_pi(n)){
                //std::cout<<"Pushed Node: "<<n<< std::endl;
                topo_order.push_back(n);
            }

            this->set_visited(n, this->trav_id());

            auto it = find(topo_wait.begin(), topo_wait.end(), n);
            int index;
            // If element was found
            if (it != topo_wait.end())
            {

                // calculating the index
                // of K
                index = it - topo_wait.begin();
            }
            else {
                // If the element is not
                // present in the vector
                std::cout << "-1" << std::endl;
            }

            /*Push the outputs of a finally pushed node to the queue*/
            my_ntk.foreach_fanout(n, [&](auto ok){
                                   if(this->visited(ok) != tid){
                                       topo_wait.push_back(ok);
                                   }
                                   this->set_visited(ok, tid);
                                   const auto fis = fanins(my_ntk, ok);
                                   my_bool=false;
                                   my_bool = std::all_of(fis.fanin_nodes.begin(), fis.fanin_nodes.end(), [&](const auto& fin_inp)
                                                                {return this->visited(fin_inp) == this->trav_id();});
                                   if (my_bool){
                                       next_node = ok;
                                   }
                               });


            topo_wait.erase(topo_wait.begin()+index);
            push_iter=0;
        }else{
            ++push_iter;
        }

        if(!topo_wait.empty()){
            if(my_bool){
                create_rest_topo(next_node, tid);
            }
            else create_rest_topo(topo_wait[push_iter], tid);
        }
    }

    void input_sort(node const& n){

        /*Place High Fan-out nodes first*/
        my_ntk.foreach_fanout(n, [&](const auto& fon){
                               if(my_ntk.is_fanout(fon))
                               {
                                   topo_order.push_back(n);
                                   this->set_visited(n, this->trav_id());
                                   /*Check if the fan-in-nodes on the high fan-out input*/
                                   my_ntk.foreach_fanout(fon, [&](const auto& fo)
                                                      {
                                                          const auto fe = fanins(my_ntk, fo);
                                                          all_fanins_single_inputs = std::all_of(fe.fanin_nodes.begin(), fe.fanin_nodes.begin(), [&](const auto& fin_inp)
                                                                                                 {
                                                                                                     /*FEHLER?*/
                                                                                                     if (fin_inp!=fon){
                                                                                                         return  my_ntk.is_ci(fin_inp);
                                                                                                     } else return true;

                                                                                                 });
                                                      });
                               }
                               else
                               {
                                   /*The Fan-out has to be connected to 1.Fan-out nodes of Inputs or 2.Inputs*/
                                   /*Has to be rewritten for Inputs with Fan-outs higher than two*/
                                   bool all_inputs = true;
                                   my_ntk.foreach_fanin(fon, [&](const auto& fi){
                                                         auto fin_inp = my_ntk.get_node(fi);
                                                         /*Check if it´s an inverter node*/
                                                         if (my_ntk.is_inv(fin_inp)){
                                                             const auto fis_inv = fanins(my_ntk, fin_inp);
                                                             fin_inp = fis_inv.fanin_nodes[0];
                                                         }
                                                         /*1*/
                                                         if (my_ntk.fanout_size(fin_inp)>=2){
                                                             my_ntk.foreach_fanin(fin_inp, [&](const auto& fin){
                                                                                   auto fin_inp_sec = my_ntk.get_node(fin);
                                                                                   if (my_ntk.is_inv(fin_inp_sec)){
                                                                                       const auto fis_inv = fanins(my_ntk, fin_inp_sec);
                                                                                       fin_inp_sec = fis_inv.fanin_nodes[0];
                                                                                   }
                                                                                   if(my_ntk.is_ci(fin_inp_sec) == false)
                                                                                   {
                                                                                       all_inputs = false;
                                                                                   }
                                                                               });
                                                         }
                                                         /*2*/
                                                         else if(my_ntk.is_ci(fin_inp) == false)
                                                         {
                                                             all_inputs = false;
                                                         }});
                                   /*Place inputs first, which are connected "over one node" to a high-fan-out input */
                                   if(all_inputs)
                                   {
                                       my_ntk.foreach_fanin(fon, [&](const auto& fi)
                                                         {
                                                             auto fin = my_ntk.get_node(fi);
                                                             //12 34
                                                             if( fin!=n)
                                                             {
                                                                 bool inv_flag = false;
                                                                 if (my_ntk.is_inv(fin)){
                                                                     const auto fis_inv = fanins(my_ntk, fin);
                                                                     fin = fis_inv.fanin_nodes[0];
                                                                     inv_flag = true;
                                                                 }
                                                                 if (my_ntk.fanout_size(fin) >= 2)
                                                                 {
                                                                     if(inv_flag)
                                                                     {
                                                                         wait.insert(wait.begin()+non_inv, n);
                                                                     }
                                                                     else
                                                                     {
                                                                         ++non_inv;
                                                                         wait.insert(wait.begin(), n);
                                                                     }

                                                                     /*Das gehört woanders hin*/
                                                                     /*fo_two_inv_flag = true;
                                                                     if (!all_fanins_single_inputs)
                                                                     {
                                                                         fo_two_inv_flag = false;
                                                                     }*/
                                                                     /*++++++++++++++++++++++++++++*/
                                                                 }
                                                                 else
                                                                 {
                                                                     if(single_input_iterator==0)
                                                                     {
                                                                         wait.push_back(n);
                                                                         ++single_input_iterator;
                                                                     }
                                                                     else if(single_input_iterator>=1)
                                                                     {
                                                                         if (!wait.empty())
                                                                         {
                                                                             const auto fo_node = fanouts(my_ntk ,n);
                                                                             const auto fo_wait = fanouts(my_ntk, wait.at(wait.size()-single_input_iterator));
                                                                             //std::cout<<"fo_node"<<fo_node[0]<<std::endl;

                                                                             //std::cout<<"fo_wait"<<fo_wait[0]<<std::endl;
                                                                             if(fo_node[0]==fo_wait[0]){
                                                                                 wait.insert(wait.end()-single_input_iterator+1, n);
                                                                             }

                                                                             else
                                                                             {
                                                                                 wait.push_back(n);
                                                                             }
                                                                             ++single_input_iterator;
                                                                         }
                                                                     }

                                                                 }
                                                             }
                                                             else if (my_ntk.is_inv(fin) && fin==n)
                                                             {
                                                                 fo_one_inv_flag = true;
                                                             }
                                                         });
                                   }
                                   else
                                   {
                                       wait.push_back(n);
                                   }
                               }
                           });

    }

  private:
    std::vector<node>     topo_order;
    std::optional<signal> start_signal;

    Ntk my_ntk;
    std::vector<node> topo_wait;
    uint32_t num_p;
    uint32_t num_c = 0u;
    bool fo_one_inv_flag = false;
    bool fo_two_inv_flag = false;
    unsigned int single_input_iterator = 0;
    int non_inv = 0;
    bool all_fanins_single_inputs = false;
    std::vector<node> wait;
    unsigned int push_iter;
    bool return_flag = false;
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
