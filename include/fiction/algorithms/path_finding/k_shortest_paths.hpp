//
// Created by marcel on 21.02.22.
//

#ifndef FICTION_K_SHORTEST_PATHS_HPP
#define FICTION_K_SHORTEST_PATHS_HPP

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <vector>

namespace fiction
{

struct yen_k_shortest_paths_params
{};

namespace detail
{

template <typename Path, typename Lyt>
class yen_k_shortest_paths_impl
{
  public:
    yen_k_shortest_paths_impl(const Lyt& lyt, const coordinate<Lyt>& src, const coordinate<Lyt>& tgt, const uint32_t k,
                              const yen_k_shortest_paths_params p) :
            layout{lyt},
            source{src},
            target{tgt},
            num_shortest_paths{k},
            ps{p}
    {
        // start by determining the shortest path between source and target
        k_shortest_paths.push_back(a_star<Path>(layout, source, target));
    }

    path_collection<Path> run()
    {
        // if there was no path to begin with
        if (k_shortest_paths.back().empty())
        {
            return {};
        }

        // for the number of shortest paths k
        for (uint32_t k = 1; k < num_shortest_paths; ++k)
        {
            const auto& latest_path = k_shortest_paths[k - 1];

            // for all coordinates of the latest path except the last one
            for (uint32_t i = 0; i < static_cast<int64_t>(latest_path.size()) - 1; ++i)
            {
                // create a spur, which is the ith coordinate of the latest path
                const auto spur = latest_path[i];

                // the root path is the path from the source to the spur coordinate
                Path root_path{latest_path.cbegin(), latest_path.cbegin() + i};

                // for all previous paths
                for (const auto& p : k_shortest_paths)
                {
                    // if the root path is equal to a previous partial path
                    if (std::equal(root_path.cbegin(), root_path.cend(), p.cbegin(), p.cbegin() + i))
                    {
                        // block the connection that was already used in the previous shortest path
                        layout.obstruct_connection(p[i], p[i + 1]);
                    }
                }

                // for all coordinates in the root path...
                for (const auto& root : root_path)
                {
                    // ... that are not the spur
                    if (root != spur)
                    {
                        // block them from further exploration
                        layout.obstruct_coordinate(root);
                    }
                }

                // find an alternative path from the spur coordinate to the target and check that it is not empty
                if (auto spur_path = a_star<Path>(layout, spur, target); !spur_path.empty())
                {
                    // the final path will be a concatenation of the root path and the spur path
                    auto& final_path = root_path;
                    // allocate more memory for the final path (prepare concatenation)
                    final_path.reserve(root_path.size() + spur_path.size());
                    // concatenate root path and spur path to get the final path
                    final_path.insert(final_path.end(), std::make_move_iterator(spur_path.begin()),
                                      std::make_move_iterator(spur_path.end()));

                    // if the candidates do not already contain the path, it is a potential k-shortest path
                    if (!final_path.empty())  // NOTE a contains check needs to be added back in if no set is used here
                    {
                        shortest_path_candidates.add(final_path);
                    }
                }

                // clear obstructions again (prepare for the next potential path)
                layout.clear_obstructed_coordinates();
                layout.clear_obstructed_connections();
            }

            // if there were no spur paths or if all spur paths have been added to k_shortest_paths already
            if (shortest_path_candidates.empty())
            {
                break;
            }

            // fetch and remove the lowest cost path from the candidates and add it to k_shortest_paths
            if (const auto lowest_cost_path_it =
                    std::min_element(shortest_path_candidates.cbegin(), shortest_path_candidates.cend(),
                                     [](const auto& p1, const auto& p2) { return path_cost(p1) < path_cost(p2); });
                lowest_cost_path_it != shortest_path_candidates.cend())
            {
                k_shortest_paths.add(*lowest_cost_path_it);
                shortest_path_candidates.erase(lowest_cost_path_it);
            }
        }

        return k_shortest_paths;
    }

  private:
    /**
     * The layout in which k shortest paths are to be found extended by an obstruction functionality layer.
     */
    obstruction_layout<Lyt> layout;
    /**
     * Source and target coordinates.
     */
    const coordinate<Lyt>&source, target;
    /**
     * The number of paths to determine, i.e., k.
     */
    const uint32_t num_shortest_paths;
    /**
     * Parameters.
     */
    yen_k_shortest_paths_params ps;
    /**
     * The list of k shortest paths that is created during the algorithm.
     */
    path_collection<Path> k_shortest_paths{};
    /**
     * A set of potential shortest paths.
     */
    path_set<Path> shortest_path_candidates{};
    /**
     * Computes the cost of a path. This function can be adjusted to fetch paths of differing costs.
     *
     * Currently, the cost is equal to its length.
     *
     * @param p Path whose costs are to be calculated.
     * @return Costs of path p.
     */
    static std::size_t path_cost(const Path& p) noexcept
    {
        return p.size();
    }
};

}  // namespace detail

/**
 * Yen's algorithm for finding up to k shortest paths without loops from source to target. This implementation works on
 * clocked layouts and uses the A* algorithm with the Manhattan distance function internally. The algorithm was
 * originally described in "An algorithm for finding shortest routes from all source nodes to a given destination in
 * general networks" by Jin Y. Yen in Quarterly of Applied Mathematics, 1970.
 *
 * If k is larger than the number of possible paths from source to target, the size of the returned path collection will
 * be smaller than k.
 *
 * If the given layout implements the obstruction interface (see obstruction_layout), paths will not be routed via
 * obstructed coordinates.
 *
 * @tparam Path Path type to create.
 * @tparam Lyt Clocked layout type.
 * @param layout The clocked layout in which the k shortest paths between source and target are to be found.
 * @param source Starting coordinate.
 * @param target Goal coordinate.
 * @param k Maximum number of shortest paths to find.
 * @param ps Parameters.
 * @return A collection of up to k shortest loopless paths in layout from source to target.
 */
template <typename Path, typename Lyt>
[[nodiscard]] path_collection<Path> yen_k_shortest_paths(const Lyt& layout, const coordinate<Lyt>& source,
                                                         const coordinate<Lyt>& target, const uint32_t k,
                                                         yen_k_shortest_paths_params ps = {}) noexcept
{
    static_assert(is_clocked_layout_v<Lyt>, "Lyt is not a clocked layout");

    return detail::yen_k_shortest_paths_impl<Path, Lyt>{layout, source, target, k, ps}.run();
}

}  // namespace fiction

#endif  // FICTION_K_SHORTEST_PATHS_HPP
