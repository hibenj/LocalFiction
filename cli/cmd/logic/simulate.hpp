//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_SIMULATE_HPP
#define FICTION_CMD_SIMULATE_HPP

#include <fiction/types.hpp>
#include <fiction/utils/name_utils.hpp>

#include <alice/alice.hpp>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <nlohmann/json.hpp>

#include <memory>
#include <string>

namespace alice
{
/**
 * Simulates network or gate-level layout into truth tables.
 *
 * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/algorithms/simulate.hpp.
 */
class simulate_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit simulate_command(const environment::ptr& e) :
            command(e, "Computes truth tables for each primary output of the current logic network or "
                       "gate-level layout in store.")
    {
        add_flag("--gate_layout,-g", "Simulate gate layout");
        add_flag("--network,-n", "Simulate logic network");
        add_flag("--store", "Store simulation results in truth table store");
        add_flag("--silent", "Do not print truth tables");
    }

  protected:
    /**
     * Function to perform the simulate call. Computes truth tables.
     */
    void execute() override
    {
        if (is_set("gate_layout") == is_set("network"))
        {
            env->out() << "[w] exactly one store needs to be specified" << std::endl;
            return;
        }

        tables.clear();
        po_names.clear();

        if (is_set("gate_layout"))
        {
            auto& gls = store<fiction::gate_layout_t>();

            // error case: empty gate layout store
            if (gls.empty())
            {
                env->out() << "[w] no gate layout in store" << std::endl;
                return;
            }

            perform_simulation(gls.current());
        }
        else if (is_set("network"))
        {
            auto& lns = store<fiction::logic_network_t>();

            // error case: empty logic network store
            if (lns.empty())
            {
                env->out() << "[w] no logic network in store" << std::endl;
                return;
            }

            perform_simulation(lns.current());
        }

        auto& tts = store<fiction::truth_table_t>();
        for (auto i = 0ul; i < tables.size(); ++i)
        {
            if (is_set("store"))
            {
                tts.extend() = std::make_shared<fiction::tt>(tables[i]);
            }
            if (!is_set("silent"))
            {
                env->out() << fmt::format("[i] {} - hex: {}, bin: {}", po_names[i], kitty::to_hex(tables[i]),
                                          kitty::to_binary(tables[i]))
                           << std::endl;
            }
        }
    }

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the truth tables.
     */
    nlohmann::json log() const override
    {
        nlohmann::json j;
        for (auto i = 0ul; i < tables.size(); ++i)
        {
            j.push_back({{"po", po_names[i]},
                         {"vars", tables[i].num_vars()},
                         {"hex", kitty::to_hex(tables[i])},
                         {"binary", kitty::to_binary(tables[i])}});
        }

        return {{"tables", j}};
    }

  private:
    /**
     * Stores truth tables.
     */
    std::vector<fiction::tt> tables;
    /**
     * Stores PO names in order.
     */
    std::vector<std::string> po_names;

    template <typename NtkOrLytVariant>
    void perform_simulation(const NtkOrLytVariant& network_or_layout_variant)
    {
        const auto get_name = [](auto&& ntk_or_lyt_ptr) -> std::string { return fiction::get_name(*ntk_or_lyt_ptr); };

        const auto store_po_names = [this](auto&& ntk_or_lyt_ptr)
        {
            ntk_or_lyt_ptr->foreach_po(
                [this, &ntk_or_lyt_ptr]([[maybe_unused]] const auto& po, auto i)
                {
                    po_names.emplace_back(ntk_or_lyt_ptr->has_output_name(i) ? ntk_or_lyt_ptr->get_output_name(i) :
                                                                               fmt::format("po{}", i));
                });
        };

        const auto simulate = [this](auto&& ntk_or_lyt_ptr)
        {
            tables = mockturtle::simulate<fiction::tt>(
                *ntk_or_lyt_ptr,
                // NOLINTNEXTLINE
                mockturtle::default_simulator<fiction::tt>{static_cast<unsigned>(ntk_or_lyt_ptr->num_pis())});
        };

        std::visit(store_po_names, network_or_layout_variant);

        try
        {
            std::visit(simulate, network_or_layout_variant);
        }
        catch (const std::bad_alloc&)
        {
            env->out() << "[e] " << std::visit(get_name, network_or_layout_variant)
                       << " has too many inputs to store its truth table" << std::endl;
            return;
        }
    }
};

ALICE_ADD_COMMAND(simulate, "Logic")

}  // namespace alice

#endif  // FICTION_CMD_SIMULATE_HPP
