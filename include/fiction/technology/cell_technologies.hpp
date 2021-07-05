//
// Created by marcel on 23.06.21.
//

#ifndef FICTION_CELL_TECHNOLOGIES_HPP
#define FICTION_CELL_TECHNOLOGIES_HPP

#include <cstdint>

namespace fiction
{
/**
 * Supported technology types:
 * - Qunatum-dot Cellular Automata (QCA)
 * - In-plane Nanomagnet Logic (iNML)
 * - Silicon Dangling Bonds (SiDB)
 */
enum class technology_implementation
{
    QCA,
    iNML,
    SiDB
};
/**
 * Overload for the stream operator on technology_implementation to print a string representation.
 *
 * @param os Stream to write into.
 * @param tech Technology to print.
 * @return os extended by string representation of tech.
 */
std::ostream& operator<<(std::ostream& os, const technology_implementation tech)
{
    switch (tech)
    {
        case technology_implementation::QCA:
        {
            os << "QCA";
            break;
        }
        case technology_implementation::iNML:
        {
            os << "iNML";
            break;
        }
        case technology_implementation::SiDB:
        {
            os << "SiDB";
            break;
        }
        default:
        {
            os << "?";
            break;
        }
    }

    return os;
}

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
        CONST_1 = '1',
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

    static constexpr const technology_implementation implementation = technology_implementation::QCA;

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
};

}  // namespace fiction

#endif  // FICTION_CELL_TECHNOLOGIES_HPP