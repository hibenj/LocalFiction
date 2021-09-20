//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_STORES_HPP
#define FICTION_STORES_HPP

#include <fiction/io/dot_drawers.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/io/write_svg_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <type_traits>
#include <variant>

namespace alice
{
/**
 * Truth tables.
 *
 * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/stores/tt.hpp.
 */
ALICE_ADD_STORE(fiction::truth_table_t, "truth_table", "t", "truth table", "truth tables")

ALICE_DESCRIBE_STORE(fiction::truth_table_t, tt)
{
    if (tt->num_vars() <= 6)
        return fmt::format("{} vars, hex: {}, bin: {}", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
    else
        return fmt::format("{} vars, (description omitted due to truth table size)", tt->num_vars());
}

ALICE_PRINT_STORE_STATISTICS(fiction::truth_table_t, os, tt)
{
    if (tt->num_vars() <= 6)
        os << fmt::format("[i] {} vars, hex: {}, bin: {}\n", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
    else
        os << fmt::format("[i] {} vars, (description omitted due to truth table size)\n", tt->num_vars());
}

ALICE_LOG_STORE_STATISTICS(fiction::truth_table_t, tt)
{
    return {{"vars", tt->num_vars()}, {"hex", kitty::to_hex(*tt)}, {"binary", kitty::to_binary(*tt)}};
}

ALICE_PRINT_STORE(fiction::truth_table_t, os, tt)
{
    os << fmt::format("[i] {} vars, hex: {}, bin: {}", tt->num_vars(), kitty::to_hex(*tt), kitty::to_binary(*tt));
}

/**
 * Logic networks.
 */
ALICE_ADD_STORE(fiction::logic_network_t, "network", "n", "logic network", "logic networks")

ALICE_DESCRIBE_STORE(fiction::logic_network_t, ln)
{
    const auto describe = [](auto&& ntk_ptr)
    {
        using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;

        mockturtle::depth_view depth_ntk{*ntk_ptr};

        return fmt::format("{} ({}) - I/O: {}/{}, gates: {}, level: {}", ntk_ptr->get_network_name(),
                           fiction::ntk_type_name<Ntk>, ntk_ptr->num_pis(), ntk_ptr->num_pos(), ntk_ptr->num_gates(),
                           depth_ntk.depth());
    };

    return std::visit(describe, ln);
}

ALICE_PRINT_STORE_STATISTICS(fiction::logic_network_t, os, ln)
{
    const auto print_statistics = [&os](auto&& ntk_ptr)
    {
        using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;

        mockturtle::depth_view depth_ntk{*ntk_ptr};

        os << fmt::format("[i] {} ({}) - I/O: {}/{}, gates: {}, level: {}\n", ntk_ptr->get_network_name(),
                          fiction::ntk_type_name<Ntk>, ntk_ptr->num_pis(), ntk_ptr->num_pos(), ntk_ptr->num_gates(),
                          depth_ntk.depth());
    };

    std::visit(print_statistics, ln);
}

ALICE_LOG_STORE_STATISTICS(fiction::logic_network_t, ln)
{
    const auto log_statistics = [](auto&& ntk_ptr)
    {
        using Ntk = typename std::decay_t<decltype(ntk_ptr)>::element_type;

        mockturtle::depth_view depth_ntk{*ntk_ptr};

        return nlohmann::json{{"name", ntk_ptr->get_network_name()}, {"type", fiction::ntk_type_name<Ntk>},
                              {"inputs", ntk_ptr->num_pis()},        {"outputs", ntk_ptr->num_pos()},
                              {"gates", ntk_ptr->num_gates()},       {"level", depth_ntk.depth()}};
    };

    return std::visit(log_statistics, ln);
}

template <>
bool can_show<fiction::logic_network_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    extension = "dot";

    // indexes for both logic networks and gate layouts
    cmd.add_flag("--indexes,-i", "Show node indexes")->group("logic_network (-n) / gate_layout (-g)");

    return true;
}

template <>
void show<fiction::logic_network_t>(std::ostream& os, const fiction::logic_network_t& element, const command& cmd)
{
    const auto show_ntk = [&os, &cmd](auto&& ntk_ptr)
    {
        try
        {
            mockturtle::depth_view depth_ntk{*ntk_ptr};

            using Ntk = typename std::decay_t<decltype(depth_ntk)>;

            if (cmd.is_set("indexes"))
            {
                mockturtle::write_dot(depth_ntk, os, fiction::technology_dot_drawer<Ntk, true>());
            }
            else
            {
                mockturtle::write_dot(depth_ntk, os, fiction::technology_dot_drawer<Ntk, false>());
            }
        }
        catch (const std::invalid_argument& e)
        {
            cmd.env->out() << "[e] " << e.what() << std::endl;
        }
    };

    std::visit(show_ntk, element);
}

/**
 * FCN gate-level layouts.
 */
ALICE_ADD_STORE(fiction::gate_layout_t, "gate_layout", "g", "gate layout", "gate layouts")

ALICE_PRINT_STORE(fiction::gate_layout_t, os, layout)
{
    const auto print = [&os](auto&& lyt_ptr) { fiction::print_gate_level_layout(os, *lyt_ptr); };

    std::visit(print, layout);
}

ALICE_DESCRIBE_STORE(fiction::gate_layout_t, layout)
{
    const auto describe = [](auto&& lyt_ptr)
    {
        return fmt::format("{} - {} × {}, I/O: {}/{}, gates: {}, wires: {}", lyt_ptr->get_layout_name(),
                           lyt_ptr->x() + 1, lyt_ptr->y() + 1, lyt_ptr->num_pis(), lyt_ptr->num_pos(),
                           lyt_ptr->num_gates(), lyt_ptr->num_wires());
    };

    return std::visit(describe, layout);
}

ALICE_PRINT_STORE_STATISTICS(fiction::gate_layout_t, os, layout)
{
    // TODO crossings, critical path, throughput
    const auto print_statistics = [&os](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        uint32_t num_se{0};

        if constexpr (fiction::has_synchronization_elements_v<Lyt>)
        {
            num_se = lyt_ptr->num_se();
        }

        mockturtle::depth_view depth_lyt{*lyt_ptr};

        os << fmt::format("[i] {} - {} × {}, I/O: {}/{}, gates: {}, wires: {}, CP: {}, sync. elems.: {}\n",
                          lyt_ptr->get_layout_name(), lyt_ptr->x() + 1, lyt_ptr->y() + 1, lyt_ptr->num_pis(),
                          lyt_ptr->num_pos(), lyt_ptr->num_gates(), lyt_ptr->num_wires(), depth_lyt.depth(), num_se);
    };

    std::visit(print_statistics, layout);
}

ALICE_LOG_STORE_STATISTICS(fiction::gate_layout_t, layout)
{
    const auto log_statistics = [](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        uint32_t num_se{0};

        if constexpr (fiction::has_synchronization_elements_v<Lyt>)
        {
            num_se = lyt_ptr->num_se();
        }

        mockturtle::depth_view depth_lyt{*lyt_ptr};

        return nlohmann::json{
            {"name", lyt_ptr->get_layout_name()},
            {"inputs", lyt_ptr->num_pis()},
            {"outputs", lyt_ptr->num_pos()},
            {"gates", lyt_ptr->num_gates()},
            {"wires", lyt_ptr->num_wires()},
            {"layout", {{"x-size", lyt_ptr->x() + 1}, {"y-size", lyt_ptr->y() + 1}, {"area", lyt_ptr->area()}}},
            // {"bounding box", {{"x-size", bb.x_size}, {"y-size", bb.y_size}, {"area", bb.area()}}},
            // {"free tiles", area - (gate_tiles + wire_tiles - crossings)},  // free tiles in ground layer
            // {"crossings", crossings},
            {"synchronization elements", num_se},
            {"critical path", depth_lyt.depth()}
            // {"throughput", fmt::format("1/{}", tp)}};
        };
    };

    return std::visit(log_statistics, layout);
}

template <>
bool can_show<fiction::gate_layout_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    extension = "dot";

    // already added for logic network; alice doesn't allow for both
    // cmd.add_flag("--indexes,-i", "Show node indexes")->group("gate_layout (-g)");

    return true;
}

template <>
void show<fiction::gate_layout_t>(std::ostream& os, const fiction::gate_layout_t& element, const command& cmd)
{
    const auto show_lyt = [&os, &cmd](auto&& lyt_ptr)
    {
        try
        {
            using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

            if (cmd.is_set("indexes"))
            {
                fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_cartesian_drawer<Lyt, true>());
            }
            else
            {
                fiction::write_dot_layout(*lyt_ptr, os, fiction::gate_layout_cartesian_drawer<Lyt, false>());
            }
        }
        catch (const std::invalid_argument& e)
        {
            cmd.env->out() << "[e] " << e.what() << std::endl;
        }
    };

    std::visit(show_lyt, element);
}

/**
 * FCN cell-level layouts.
 */
ALICE_ADD_STORE(fiction::cell_layout_t, "cell_layout", "c", "cell layout", "cell layouts")

ALICE_PRINT_STORE(fiction::cell_layout_t, os, layout)
{
    const auto print = [&os](auto&& lyt_ptr) { fiction::print_cell_level_layout(os, *lyt_ptr); };

    std::visit(print, layout);
}

ALICE_DESCRIBE_STORE(fiction::cell_layout_t, layout)
{
    const auto describe = [](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        return fmt::format("{} ({}) - {} × {}, I/O: {}/{}, cells: {}", lyt_ptr->get_layout_name(),
                           fiction::tech_impl_name<fiction::technology<Lyt>>, lyt_ptr->x() + 1, lyt_ptr->y() + 1,
                           lyt_ptr->num_pis(), lyt_ptr->num_pos(), lyt_ptr->num_cells());
    };

    return std::visit(describe, layout);
}

ALICE_PRINT_STORE_STATISTICS(fiction::cell_layout_t, os, layout)
{
    const auto print_statistics = [&os](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        os << fmt::format("[i] {} ({}) - {} × {}, I/O: {}/{}, cells: {}\n", lyt_ptr->get_layout_name(),
                          fiction::tech_impl_name<fiction::technology<Lyt>>, lyt_ptr->x() + 1, lyt_ptr->y() + 1,
                          lyt_ptr->num_pis(), lyt_ptr->num_pos(), lyt_ptr->num_cells());
    };

    std::visit(print_statistics, layout);
}

ALICE_LOG_STORE_STATISTICS(fiction::cell_layout_t, layout)
{
    const auto log_statistics = [](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        return nlohmann::json{
            {"name", lyt_ptr->get_layout_name()},
            {"technology", fiction::tech_impl_name<fiction::technology<Lyt>>},
            {"inputs", lyt_ptr->num_pis()},
            {"outputs", lyt_ptr->num_pos()},
            {"cells", lyt_ptr->num_cells()},
            {"layout", {{"x-size", lyt_ptr->x() + 1}, {"y-size", lyt_ptr->y() + 1}, {"area", lyt_ptr->area()}}}};
    };

    return std::visit(log_statistics, layout);
}

template <>
bool can_show<fiction::cell_layout_t>(std::string& extension, [[maybe_unused]] command& cmd)
{
    cmd.add_flag("--simple,-s", "Simplified depiction abstracting from details")->group("cell_layouts (-c)");

    extension = "svg";

    return true;
}

template <>
void show<fiction::cell_layout_t>(std::ostream& os, const fiction::cell_layout_t& element,
                                  const command& cmd)  // const & for pointer because alice says so...
{
    const auto show_lyt = [&os, &cmd](auto&& lyt_ptr)
    {
        using Lyt = typename std::decay_t<decltype(lyt_ptr)>::element_type;

        if constexpr (!std::is_same_v<fiction::technology<Lyt>, fiction::qca_technology>)
        {
            cmd.env->out() << fmt::format("[e] {} is not a QCA layout", lyt_ptr->get_layout_name()) << std::endl;
        }
        else
        {
            try
            {
                fiction::write_qca_layout_svg(*lyt_ptr, os, {cmd.is_set("simple")});
            }
            catch (const fiction::unsupported_cell_type_exception<fiction::coordinate<Lyt>>& e)
            {
                cmd.env->out() << fmt::format("[e] unsupported cell type at cell position {}", e.where()) << std::endl;
            }
            catch (...)
            {
                cmd.env->out() << "[e] an error occurred while writing the SVG description to the output stream"
                               << std::endl;
            }
        }
    };

    std::visit(show_lyt, element);
}

}  // namespace alice

#endif  // FICTION_STORES_HPP
