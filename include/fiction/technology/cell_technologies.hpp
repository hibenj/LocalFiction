//
// Created by marcel on 23.06.21.
//

#ifndef FICTION_CELL_TECHNOLOGIES_HPP
#define FICTION_CELL_TECHNOLOGIES_HPP

#include <cstdint>

namespace fiction
{

struct qca_technology
{
    enum cell_type : uint8_t
    {
        /**
         * Symbol used for empty QCA cells.
         */
        EMPTY = ' ',
        /**
         * Symbol used for normal QCA cells.
         */
        NORMAL = 'x',
        /**
         * Symbol used for input QCA cells.
         */
        INPUT = 'i',
        /**
         * Symbol used for output QCA cells.
         */
        OUTPUT = 'o',
        /**
         * Symbol used for constant 0 input QCA cells.
         */
        CONST_0 = '0',
        /**
         * Symbol used for constant 1 input QCA cells.
         */
        CONST_1 = '1'
    };
    /**
     * Possible cell modes for QCA cells.
     */
    enum class cell_mode : uint8_t
    {
        NORMAL = 0u,
        ROTATED,
        VERTICAL,
        CROSSOVER
    };
    /**
     * Possible marks to be applied to a cell to change its type.
     */
    enum class cell_mark : uint8_t
    {
        EMPTY  = cell_type::EMPTY,
        INPUT  = cell_type::INPUT,
        OUTPUT = cell_type::OUTPUT
    };

    [[nodiscard]] static constexpr bool is_empty_cell(const cell_type& c) noexcept
    {
        return c == EMPTY;
    }

    [[nodiscard]] static constexpr bool is_normal_cell(const cell_type& c) noexcept
    {
        return c == NORMAL;
    }

    [[nodiscard]] static constexpr bool is_input_cell(const cell_type& c) noexcept
    {
        return c == INPUT;
    }

    [[nodiscard]] static constexpr bool is_output_cell(const cell_type& c) noexcept
    {
        return c == OUTPUT;
    }

    [[nodiscard]] static constexpr bool is_const_0_cell(const cell_type& c) noexcept
    {
        return c == CONST_0;
    }

    [[nodiscard]] static constexpr bool is_const_1_cell(const cell_type& c) noexcept
    {
        return c == CONST_1;
    }

    [[nodiscard]] static constexpr bool is_constant_cell(const cell_type& c) noexcept
    {
        return is_const_0_cell(c) || is_const_1_cell(c);
    }

    [[nodiscard]] static constexpr bool is_normal_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::NORMAL;
    }

    [[nodiscard]] static constexpr bool is_rotated_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::ROTATED;
    }

    [[nodiscard]] static constexpr bool is_vertical_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::VERTICAL;
    }

    [[nodiscard]] static constexpr bool is_crossover_cell_mode(const cell_mode& m) noexcept
    {
        return m == cell_mode::CROSSOVER;
    }
    /**
     * Default width of a QCA cell in QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/).
     */
    static constexpr uint64_t cell_width = 18ul;
    /**
     * Default height of a QCA cell in QCADesigner.
     */
    static constexpr uint64_t cell_height = 18ul;
    /**
     * Default horizontal spacing between two QCA cells in QCADesigner.
     */
    static constexpr uint64_t cell_hspace = 2ul;
    /**
     * Default vertical spacing between two QCA cells in QCADesigner.
     */
    static constexpr uint64_t cell_vspace = 2ul;
};

struct inml_technology
{
    enum cell_type : uint8_t
    {
        /**
         * Symbol used for empty iNML cells.
         */
        EMPTY = ' ',
        /**
         * Symbol used for normal iNML cells.
         */
        NORMAL = 'x',
        /**
         * Symbol used for input iNML cells.
         */
        INPUT = 'i',
        /**
         * Symbol used for output iNML cells.
         */
        OUTPUT = 'o',
        /**
         * Symbol used for upper slanted edge magnets.
         */
        SLANTED_EDGE_UP_MAGNET = 'u',
        /**
         * Symbol used for lower slanted edge magnets.
         */
        SLANTED_EDGE_DOWN_MAGNET = 'd',
        /**
         * Symbol used for inverter magnets.
         */
        INVERTER_MAGNET = 'n',
        /**
         * Symbol used for cross-wire magnets.
         */
        CROSSWIRE_MAGNET = 'c',
        /**
         * Symbol used for coupler (fan-out) magnets.
         */
        FANOUT_COUPLER_MAGNET = 'f'
    };

    /**
     * iNML cells do not have modes.
     */
    struct cell_mode
    {};

    [[nodiscard]] static constexpr bool is_empty_cell(const cell_type& c) noexcept
    {
        return c == EMPTY;
    }

    [[nodiscard]] static constexpr bool is_normal_cell(const cell_type& c) noexcept
    {
        return c == NORMAL;
    }

    [[nodiscard]] static constexpr bool is_input_cell(const cell_type& c) noexcept
    {
        return c == INPUT;
    }

    [[nodiscard]] static constexpr bool is_output_cell(const cell_type& c) noexcept
    {
        return c == OUTPUT;
    }

    [[nodiscard]] static constexpr bool is_slanted_edge_up_magnet(const cell_type& c) noexcept
    {
        return c == SLANTED_EDGE_UP_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_slanted_edge_down_magnet(const cell_type& c) noexcept
    {
        return c == SLANTED_EDGE_DOWN_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_slanted_edge_magnet(const cell_type& c) noexcept
    {
        return is_slanted_edge_up_magnet(c) || is_slanted_edge_down_magnet(c);
    }

    [[nodiscard]] static constexpr bool is_inverter_magnet(const cell_type& c) noexcept
    {
        return c == INVERTER_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_crosswire_magnet(const cell_type& c) noexcept
    {
        return c == CROSSWIRE_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_fanout_coupler_magnet(const cell_type& c) noexcept
    {
        return c == FANOUT_COUPLER_MAGNET;
    }

    [[nodiscard]] static constexpr bool is_normal_cell_mode([[maybe_unused]] const cell_mode& m) noexcept
    {
        return true;
    }

    /**
     * Default width of a iNML cell in NMLSim.
     */
    static constexpr uint64_t cell_width = 50ul;
    /**
     * Default height of a iNML cell in NMLSim.
     */
    static constexpr uint64_t cell_height = 100ul;
    /**
     * Default horizontal spacing between two iNML cells in NMLSim.
     */
    static constexpr uint64_t cell_hspace = 10ul;
    /**
     * Default vertical spacing between two iNML cells in NMLSim.
     */
    static constexpr uint64_t cell_vspace = 25ul;
};

struct sidb_technology
{
    enum cell_type : uint8_t
    {
        /**
         * Symbol used for empty QCA cells.
         */
        EMPTY = ' ',
        /**
         * Symbol used for normal QCA cells.
         */
        NORMAL = 'x',
        /**
         * Symbol used for input QCA cells.
         */
        INPUT = 'i',
        /**
         * Symbol used for output QCA cells.
         */
        OUTPUT = 'o'
    };

    /**
     * SiDB cells do not have modes.
     */
    struct cell_mode
    {};

    [[nodiscard]] static constexpr bool is_empty_cell(const cell_type& c) noexcept
    {
        return c == EMPTY;
    }

    [[nodiscard]] static constexpr bool is_normal_cell(const cell_type& c) noexcept
    {
        return c == NORMAL;
    }

    [[nodiscard]] static constexpr bool is_input_cell(const cell_type& c) noexcept
    {
        return c == INPUT;
    }

    [[nodiscard]] static constexpr bool is_output_cell(const cell_type& c) noexcept
    {
        return c == OUTPUT;
    }

    [[nodiscard]] static constexpr bool is_normal_cell_mode([[maybe_unused]] const cell_mode& m) noexcept
    {
        return true;
    }

    /**
     * Default width of a SiDB in SiQAD.
     * Dots are considered to be 0-dimensional entities for simulation purposes.
     */
    static constexpr double cell_width = 0.0;
    /**
     * Default height of a SiDB in SiQAD.
     * Dots are considered to be 0-dimensional entities for simulation purposes.
     */
    static constexpr double cell_height = 0.0;
    /**
     * Default horizontal spacing between two SiDBs in SiQAD.
     */
    static constexpr double cell_hspace = 0.384;
    /**
     * Default average vertical spacing between two SiDBs in SiQAD.
     * Depending on their lattice, they can be closer together or further apart.
     */
    static constexpr double cell_vspace = 0.384;
};

}  // namespace fiction

#endif  // FICTION_CELL_TECHNOLOGIES_HPP
