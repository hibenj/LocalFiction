//
// Created by Hien Benjamin on 27.06.2022.
//

#ifndef FICTION_FO_INV_BALANCING_HPP
#define FICTION_FO_INV_BALANCING_HPP
#include "fiction/algorithms/network_transformation/network_conversion.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

namespace detail
{

template <typename NtkDest, typename NtkSrc>
class inv_balancing_impl
{
  public:
    inv_balancing_impl(const NtkSrc& src) :
            ntk{convert_network<NtkDest>(src)},
            ntk_topo{ntk},
            ntk_depth{ntk}
    {}

    NtkDest run()
    {
        // initialize a network copy
        auto init = mockturtle::initialize_copy_network<NtkDest>(ntk);

        auto& balanced = init.first;
        auto& old2new  = init.second;

        ntk_topo.foreach_gate(
            [this, &balanced, &old2new](const auto& n)
            {
                // gather children, but substitute fanins by buf where applicable
                std::vector<typename mockturtle::topo_view<NtkDest>::signal> children{};

                ntk_topo.foreach_fanin(n,
                                       [this, &n, &balanced, &old2new, &children](const auto& f)
                                       {
                                           const auto fn = ntk_topo.get_node(f);

                                           auto child = old2new[fn];

                                           children.push_back(child);
                                       });

                // clone the node with new children according to its depth
                //old2new[n] = balanced.clone_node(ntk_topo, n, children);
            });

        // restore signal names if applicable
        //fiction::restore_names(ntk_topo, balanced, old2new);

        return balanced;
    }

  private:
    NtkDest ntk;

    mockturtle::topo_view<NtkDest>  ntk_topo;
    mockturtle::depth_view<NtkDest> ntk_depth;
};


}  // namespace detail

/**
 * Substitutes high-output degrees in a logic network with fanout nodes that compute the identity function. For this
 * purpose, create_buf is utilized. Therefore, NtkDest should support identity nodes. If it does not, no new nodes will
 * in fact be created. In either case, the returned network will be logically equivalent to the input one.
 *
 * The process is rather naive with two possible strategies to pick from: breath-first and depth-first. The former
 * creates partially balanced fanout trees while the latter leads to fanout chains. Further parameterization includes
 * thresholds for the maximum number of output each node and fanout is allowed to have.
 *
 * The returned network is newly created from scratch because its type NtkDest may differ from NtkSrc.
 *
 * NOTE: The physical design algorithms natively provided in fiction do not require their input networks to be
 * fanout-substituted. If that is necessary, they will do it themselves. Providing already substituted networks does
 * however allows for the control over maximum output degrees.
 *
 * @tparam NtkDest Type of the returned logic network.
 * @tparam NtkSrc Type of the input logic network.
 * @param ntk_src The input logic network.
 * @param ps Parameters.
 * @return A fanout-substituted logic network of type NtkDest that is logically equivalent to ntk_src.*/



template <typename NtkDest, typename NtkSrc>
NtkDest inv_balancing(const NtkSrc& ntk_src)
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    static_assert(mockturtle::has_is_constant_v<NtkDest>, "NtkSrc does not implement the is_constant function");
    static_assert(mockturtle::has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi function");
    static_assert(mockturtle::has_create_not_v<NtkDest>, "NtkDest does not implement the create_not function");
    static_assert(mockturtle::has_create_po_v<NtkDest>, "NtkDest does not implement the create_po function");
    static_assert(mockturtle::has_create_buf_v<NtkDest>, "NtkDest does not implement the create_buf function");
    static_assert(mockturtle::has_clone_node_v<NtkDest>, "NtkDest does not implement the clone_node function");
    static_assert(mockturtle::has_fanout_size_v<NtkDest>, "NtkDest does not implement the fanout_size function");
    static_assert(mockturtle::has_foreach_gate_v<NtkDest>, "NtkDest does not implement the foreach_gate function");
    static_assert(mockturtle::has_foreach_fanin_v<NtkDest>, "NtkDest does not implement the foreach_fanin function");
    static_assert(mockturtle::has_foreach_po_v<NtkDest>, "NtkDest does not implement the foreach_po function");

    detail::inv_balancing_impl<NtkDest, NtkSrc> p{ntk_src};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_FO_INV_BALANCING_HPP
