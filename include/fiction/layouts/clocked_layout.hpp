//
// Created by marcel on 14.05.21.
//

#ifndef FICTION_CLOCKED_LAYOUT_HPP
#define FICTION_CLOCKED_LAYOUT_HPP

#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <set>

namespace fiction
{

/**
 * A layout type to layer on top of a coordinate layout, e.g., cartesian_layout, hexagonal_layout, or tile_based_layout.
 * This type extends the layout by providing a notion of FCN clocking. To this end, it utilizes a clocking scheme that
 * assigns each coordinate in the extended coordinate layout a clock number. These clock numbers can be manually
 * overwritten if necessary.
 *
 * In the context of this layout type, coordinates are renamed as clock zones.
 *
 * @tparam CoordinateLayout The coordinate layout type whose coordinates should be clocked.
 */
template <typename CoordinateLayout>
class clocked_layout : public CoordinateLayout
{
  public:
#pragma region Types and constructors

    using clock_zone = typename CoordinateLayout::coordinate;

    using clocking_scheme_t = clocking_scheme<clock_zone>;
    using clock_number_t    = typename clocking_scheme_t::clock_number;

    using degree_t = uint8_t;

    struct clocked_layout_storage
    {
        explicit clocked_layout_storage(const clocking_scheme_t& scheme) noexcept :
                clocking{std::make_shared<clocking_scheme_t>(scheme)}
        {}

        std::shared_ptr<clocking_scheme_t> clocking;
    };

    using base_type = clocked_layout;

    using storage = std::shared_ptr<clocked_layout_storage>;

    /**
     * Standard constructor. Creates a clocked layout of the given aspect ratio and clocks it via the irregular 'open'
     * clocking scheme. This scheme is intended to be used if all clock zones are to be manually assigned.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit clocked_layout(const typename CoordinateLayout::aspect_ratio& ar = {}) :
            CoordinateLayout(ar),
            strg{std::make_shared<clocked_layout_storage>(
                open_clocking<clocked_layout<CoordinateLayout>>(num_clks::FOUR))}
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
    }
    /**
     * Standard constructor. Creates a clocked layout of the given aspect ratio and clocks it via the given clocking
     * scheme.
     *
     * @param ar Highest possible position in the layout.
     * @param scheme Clocking scheme to apply to this layout.
     */
    clocked_layout(const typename CoordinateLayout::aspect_ratio& ar, const clocking_scheme_t& scheme) :
            CoordinateLayout(ar),
            strg{std::make_shared<clocked_layout_storage>(scheme)}
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
    }
    /**
     * Copy constructor from another layout's storage.
     *
     * @param s Storage of another clocked_layout.
     */
    explicit clocked_layout(std::shared_ptr<clocked_layout_storage> s) : strg{std::move(s)}
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
    }

#pragma endregion

#pragma region Clocking
    /**
     * Replaces the stored clocking scheme with the provided one.
     *
     * @param scheme New clocking scheme.
     */
    void replace_clocking_scheme(const clocking_scheme_t& scheme) noexcept
    {
        strg->clocking = std::make_shared<clocking_scheme_t>(scheme);
    }
    /**
     * Overrides a clock number in the stored scheme with the provided one.
     *
     * @param cz Clock zone to override.
     * @param cn New clock number for cz.
     */
    void assign_clock_number(const clock_zone& cz, const clock_number_t cn) noexcept
    {
        strg->clocking->override_clock_number(cz, cn);
    }
    /**
     * Returns the clock number for the given clock zone.
     *
     * @param cz Clock zone.
     * @return Clock number of cz.
     */
    [[nodiscard]] clock_number_t get_clock_number(const clock_zone& cz) const noexcept
    {
        return (*strg->clocking)(cz);
    }
    /**
     * Returns the number of clock phases in the layout. Each clock cycle is divided into n phases. In QCA, the number
     * of phases is usually 4. In iNML it is 3. However, theoretically, any number >= 3 can be utilized.
     *
     * @return The number of different clock signals in the layout.
     */
    [[nodiscard]] clock_number_t num_clocks() const noexcept
    {
        return strg->clocking->num_clocks;
    }
    /**
     * Returns whether the layout is clocked by a regular clocking scheme with no overwritten zones.
     *
     * @return True iff the layout is clocked by a regular scheme and no zones have been overwritten.
     */
    [[nodiscard]] bool is_regularly_clocked() const noexcept
    {
        return strg->clocking->is_regular();
    }
    /**
     * Compares the stored clocking scheme against the provided name. Names of pre-defined clocking schemes are given in
     * the clock_name namespace.
     *
     * @param name Clocking scheme name.
     * @return True iff the layout is clocked by a clocking scheme of name name.
     */
    [[nodiscard]] bool is_clocking_scheme(std::string&& name) const noexcept
    {
        return *strg->clocking == name;
    }
    /**
     * Returns a copy of the stored clocking scheme object.
     *
     * @return A copy of the stored clocking scheme object.
     */
    [[nodiscard]] clocking_scheme_t get_clocking_scheme() const noexcept
    {
        return *strg->clocking;
    }
    /**
     * Evaluates whether clock zone cz2 feeds information to clock zone cz1, i.e., whether cz2 is clocked with a
     * clock number that is lower by 1 modulo num_clocks().
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether its clock number is 1 lower.
     * @return True iff cz2 can feed information to cz1.
     */
    [[nodiscard]] bool is_incoming_clocked(const clock_zone& cz1, const clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        auto inc = static_cast<clock_number_t>(get_clock_number(cz2));
        auto out = static_cast<clock_number_t>(get_clock_number(cz1));

        return static_cast<clock_number_t>((get_clock_number(cz2) + static_cast<clock_number_t>(1)) % num_clocks()) ==
               get_clock_number(cz1);
    }
    /**
     * Evaluates whether clock zone cz2 accepts information from clock zone cz1, i.e., whether cz2 is clocked with a
     * clock number that is higher by 1 modulo num_clocks().
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether its clock number is 1 higher.
     * @return True iff cz2 can accept information from cz1.
     */
    [[nodiscard]] bool is_outgoing_clocked(const clock_zone& cz1, const clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        return static_cast<clock_number_t>((get_clock_number(cz1) + static_cast<clock_number_t>(1)) % num_clocks()) ==
               get_clock_number(cz2);
    }

#pragma endregion

#pragma region Iteration

    /**
     * Returns a container of type Container with all clock zones that are incoming to the given one.
     *
     * @tparam Container Container type that holds clock zones.
     * @param cz Base clock zone.
     * @return A container of type Container with all clock zones that are incoming to cz.
     */
    template <typename Container>
    [[nodiscard]] Container incoming_clocked_zones(const clock_zone& cz) const noexcept
    {
        const auto adj = CoordinateLayout::template adjacent_coordinates<Container>(cz);

        Container incoming{};

        std::copy_if(std::cbegin(adj), std::cend(adj), std::inserter(incoming, std::cend(incoming)),
                     [this, &cz](const auto& ct) { return is_incoming_clocked(cz, ct); });

        return incoming;
    }
    /**
     * Applies a function to all incoming clock zones of a given one in accordance with incoming_clocked_zones.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by mockturtle::foreach_element.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of cz's incoming clock zones.
     */
    template <typename Fn>
    void foreach_incoming_clocked_zone(const clock_zone& cz, Fn&& fn) const
    {
        const auto incoming = incoming_clocked_zones<std::set<clock_zone>>(cz);

        mockturtle::detail::foreach_element(incoming.cbegin(), incoming.cend(), fn);
    }
    /**
     * Returns a container of type Container with all clock zones that are outgoing from the given one.
     *
     * @tparam Container Container type that holds clock zones.
     * @param cz Base clock zone.
     * @return A container of type Container with all clock zones that are outgoing from cz.
     */
    template <typename Container>
    [[nodiscard]] Container outgoing_clocked_zones(const clock_zone& cz) const noexcept
    {
        const auto adj = CoordinateLayout::template adjacent_coordinates<Container>(cz);

        Container outgoing{};

        std::copy_if(std::cbegin(adj), std::cend(adj), std::inserter(outgoing, std::cend(outgoing)),
                     [this, &cz](const auto& ct) { return is_outgoing_clocked(cz, ct); });

        return outgoing;
    }
    /**
     * Applies a function to all outgoing clock zones of a given one in accordance with outgoing_clocked_zones.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by mockturtle::foreach_element.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of cz's outgoing clock zones.
     */
    template <typename Fn>
    void foreach_outgoing_clocked_zone(const clock_zone& cz, Fn&& fn) const
    {
        const auto outgoing = outgoing_clocked_zones<std::set<clock_zone>>(cz);

        mockturtle::detail::foreach_element(outgoing.cbegin(), outgoing.cend(), fn);
    }

#pragma endregion

#pragma region Structural properties
    /**
     * Returns the number of incoming clock zones to the given one.
     *
     * @param cz Base clock zone.
     * @return Number of cz's incoming clock zones.
     */
    [[nodiscard]] degree_t in_degree(const clock_zone& cz) const noexcept
    {
        return static_cast<degree_t>(incoming_clocked_zones<std::set<clock_zone>>(cz).size());
    }
    /**
     * Returns the number of outgoing clock zones from the given one.
     *
     * @param cz Base clock zone.
     * @return Number of cz's outgoing clock zones.
     */
    [[nodiscard]] degree_t out_degree(const clock_zone& cz) const noexcept
    {
        return static_cast<degree_t>(outgoing_clocked_zones<std::set<clock_zone>>(cz).size());
    }
    /**
     * Returns the number of incoming plus outgoing clock zones of the given one.
     *
     * @param cz Base clock zone.
     * @return Number of cz's incoming plus outgoing clock zones.
     */
    [[nodiscard]] degree_t degree(const clock_zone& cz) const noexcept
    {
        return static_cast<degree_t>(in_degree(cz) + out_degree(cz));
    }

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_CLOCKED_LAYOUT_HPP
