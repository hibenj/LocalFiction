//
// Created by marcel on 04.03.20.
//

#ifndef FICTION_EQUIVALENCE_CHECKING_UTILS_HPP
#define FICTION_EQUIVALENCE_CHECKING_UTILS_HPP

#include "fiction/algorithms/properties/critical_path_length_and_throughput.hpp"
#include "fiction/algorithms/verification/design_rule_violations.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fiction
{
/**
 * The different equivalence types possible:
 *  - STRONG equivalence: Spec and Impl are logically equivalent AND Impl has a throughput of 1/1
 *  - WEAK equivalence: Spec and Impl are logically equivalent BUT Impl has a throughput of 1/x with x > 1
 *  - NO equivalence: Spec and Impl are logically not equivalent OR Impl has DRVs
 */
enum class eq_type
{
    NO,
    WEAK,
    STRONG
};

struct equivalence_checking_stats
{
    /**
     * Stores the equivalence type.
     */
    eq_type eq = eq_type::NO;
    /**
     * Throughput values at which weak equivalence manifests.
     */
    int64_t tp_spec = 1ll, tp_impl = 1ll, tp_diff = 0ll;
    /**
     * Stores a possible counter example.
     */
    std::vector<bool> counter_example{};
    /**
     * Stores the runtime.
     */
    mockturtle::stopwatch<>::duration runtime{0};
    /**
     * Stores DRVs.
     */
    fiction::gate_level_drv_stats spec_drv_stats{}, impl_drv_stats{};
};

namespace detail
{

template <typename Spec, typename Impl>
class equivalence_checking_impl
{
  public:
    /**
     * Standard constructor.
     *
     * @param specification Logical specification of intended functionality.
     * @param implementation Implementation of specified functionality.
     * @param p Parameters.
     * @param st Statistics.
     */
    explicit equivalence_checking_impl(const Spec& specification, const Impl& implementation,
                                       equivalence_checking_stats& st) :
            spec{specification},
            impl{implementation},
            pst{st}
    {}

    void run() noexcept
    {
        mockturtle::stopwatch stop{pst.runtime};

        if constexpr (is_gate_level_layout_v<Spec>)
        {
            if (has_drvs(spec, &pst.spec_drv_stats))
            {
                return;
            }
        }
        if constexpr (is_gate_level_layout_v<Impl>)
        {
            if (has_drvs(impl, &pst.impl_drv_stats))
            {
                return;
            }
        }

        const auto miter = mockturtle::miter<mockturtle::klut_network>(spec, impl);

        if (miter)
        {
            mockturtle::equivalence_checking_stats st;

            const auto eq = mockturtle::equivalence_checking(*miter, {}, &st);

            if (eq.has_value())
            {
                pst.eq = *eq ? eq_type::STRONG : eq_type::NO;

                if (pst.eq == eq_type::STRONG)
                {
                    // compute TP of specification
                    if constexpr (fiction::is_gate_level_layout_v<Spec>)
                    {
                        fiction::critical_path_length_and_throughput_stats cplt_st{};
                        fiction::critical_path_length_and_throughput(spec, &cplt_st);

                        pst.tp_spec = static_cast<int64_t>(cplt_st.throughput);
                    }
                    // compute TP of implementation
                    if constexpr (fiction::is_gate_level_layout_v<Impl>)
                    {
                        fiction::critical_path_length_and_throughput_stats cplt_st{};
                        fiction::critical_path_length_and_throughput(impl, &cplt_st);

                        pst.tp_impl = static_cast<int64_t>(cplt_st.throughput);
                    }

                    pst.tp_diff = std::abs(pst.tp_spec - pst.tp_impl);

                    if (pst.tp_diff != 0)
                    {
                        pst.eq = eq_type::WEAK;
                    }
                }

                if (!(*eq))
                {
                    pst.counter_example = st.counter_example;
                }
            }
            else
            {
                std::cout << "[e] resource limit exceeded" << std::endl;
            }
        }
        else
        {
            std::cout << "[w] both networks/layouts must have the same number of primary inputs and outputs"
                      << std::endl;
        }
    }

  private:
    /**
     * Specification.
     */
    const Spec spec;
    /**
     * Implementation.
     */
    const Impl impl;

    equivalence_checking_stats& pst;

    template <typename NtkOrLyt>
    bool has_drvs(const NtkOrLyt& ntk_or_lyt, gate_level_drv_stats* stats) const noexcept
    {
        fiction::gate_level_drv_params drv_ps{};

        // suppress DRV output
        std::ostringstream null_stream{};
        drv_ps.out = &null_stream;

        gate_level_drvs(ntk_or_lyt, drv_ps, stats);

        return stats->drvs != 0;
    }
};

}  // namespace detail

/**
 * Performs SAT-based equivalence checking between a specification of type Spec and an implementation of type Impl. Both
 * Spec and Impl need to be network types (that is, gate-level layouts can be utilized as well).
 *
 * This implementation enables the comparison of two logic networks, a logic network and a gate-level layout or two
 * gate-level layouts. Since gate-level layouts have a notion of timing that logic networks do not, this function does
 * not simply prove logical equivalence but, additionally, takes timing aspects into account as well.
 *
 * Thereby, three different types of equivalences arise:
 *
 * - NO equivalence: Spec and Impl are not logically equivalent or one of them is a gate-level layout that contains DRVs
 * and, thus, cannot be checked for equivalence.
 * - WEAK equivalence: Spec and Impl are logically equivalent but either one of them is a gate-level layout with TP of
 * 1/x with x > 1 or both of them are gate-level layouts with TP 1/x and 1/y, respectively, where x != y.
 * - STRONG equivalence: Spec and Impl are logically equivalent and all involved gate-level layouts have TP of 1/1.
 *
 * This approach was first proposed in "Verification for Field-coupled Nanocomputing Circuits" by M. Walter, R. Wille,
 * F. Sill Torres. D. Große, and R. Drechsler in DAC 2020.
 *
 * @tparam Spec Specification type.
 * @tparam Impl Implementation type.
 * @param spec The specification.
 * @param impl The implementation.
 * @param pst Statistics.
 */
template <typename Spec, typename Impl>
void equivalence_checking(const Spec& spec, const Impl& impl, equivalence_checking_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Spec>, "Spec is not a network type");
    static_assert(mockturtle::is_network_type_v<Impl>, "Impl is not a network type");

    equivalence_checking_stats        st{};
    detail::equivalence_checking_impl p{spec, impl, st};

    p.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_EQUIVALENCE_CHECKING_UTILS_HPP
