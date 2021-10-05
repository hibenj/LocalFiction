//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_READ_HPP
#define FICTION_CMD_READ_HPP

#include <fiction/io/network_reader.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <string>

namespace alice
{
/**
 * Shorthand for all read commands. Chooses the proper function by the file ending.
 *
 * Currently parses Verilog, AIGER, and BLIF using the lorina parsers.
 *
 * For more information see: https://github.com/hriener/lorina
 */
class read_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit read_command(const environment::ptr& e) :
            command(e, "Reads one Verilog or AIGER file or a directory of those and creates logic network objects "
                       "which will be put into the respective store. In a directory, only files with extension "
                       "'.v' and '.aig' are considered.")
    {
        add_option("filename", filename, "Filename or directory")->required();
        add_flag("--aig,-a", "Parse networks as AIG");
        add_flag("--mig,-m", "Parse networks as MIG");
        add_flag("--top,-t", "Parse networks as Topology network");
        add_flag("--sort,-s", sort, "Sort networks in given directory by vertex count prior to storing them");
    }

  protected:
    /**
     * Function to perform the read call. Reads Verilog and creates a logic_network.
     */
    void execute() override
    {
        const auto store_nets = [&](auto&& reader)
        {
            for (const auto& ln : reader.get_networks(sort)) store<fiction::logic_network_t>().extend() = ln;
        };

        if (!is_set("aig") && !is_set("mig") && !is_set("top"))
        {
            env->out() << "[e] at least one network type must be specified" << std::endl;
        }
        else
        {
            try
            {
                if (is_set("aig"))
                {
                    fiction::network_reader<fiction::aig_ptr> reader{filename, env->out()};

                    store_nets(reader);
                }
                if (is_set("mig"))
                {
                    fiction::network_reader<fiction::mig_ptr> reader{filename, env->out()};

                    store_nets(reader);
                }
                if (is_set("top"))
                {
                    fiction::network_reader<fiction::top_ptr> reader{filename, env->out()};

                    store_nets(reader);
                }
            }
            catch (...)
            {
                env->out() << "[e] no networks were read" << std::endl;
            }
        }

        // reset flags, necessary for some reason... alice bug?
        sort = false;
    }

  private:
    /**
     * Verilog filename.
     */
    std::string filename;
    /**
     * Flag to indicate that files should be sorted by file size.
     */
    bool sort = false;
};

ALICE_ADD_COMMAND(read, "I/O")

}  // namespace alice

#endif  // FICTION_CMD_READ_HPP
