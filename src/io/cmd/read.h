//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_READ_H
#define FICTION_READ_H


#include "logic_network.h"
#include <alice/alice.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <mockturtle/io/verilog_reader.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <string>
#include <vector>


namespace alice
{
    /**
     * Shorthand for all read commands. Chooses the proper function by the file ending.
     *
     * Parses verilog files currently. Gate level netlists are supported. The lorina verilog parser is used. At the
     * moment, only 2-input gates + 3-input MAJ in assign notation are supported. Using ABC, those can be generated by
     * the commands
     *
     * read <file>
     * strash
     * write <file>.v
     *
     * For more information see: https://github.com/marcelwa/lorina
     */
    class read_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit read_command(const environment::ptr& env)
                :
                command(env,
                        "Reads one Verilog or AIGER file or a directory of those and creates logic network objects "
                        "which will be put into the respective store.\nIn a directory, only files with extension "
                        "'.v' and '.aig' are considered.")
        {
            add_option("filename", filename,
                       "Verilog or AIGER filename or directory")->required();
            add_flag("--sort,-s", sort,
                     "Sort files in given directory by file size prior to parsing");
        }

    protected:
        /**
         * Function to perform the read call. Reads Verilog and creates a logic_network.
         */
        void execute() override
        {
            // checks for extension validity
            auto is_valid_extension = [](const auto& _f)->bool
            {
                const std::vector<std::string> extensions{".v", ".aig"};
                return std::any_of(extensions.cbegin(), extensions.cend(),
                                   [&_f](const auto& _e) { return boost::filesystem::extension(_f) == _e; });
            };

            // check for for given file's properties
            if (boost::filesystem::exists(filename))
            {
                if (boost::filesystem::is_regular_file(filename))
                {
                    // collect valid files only
                    if (is_valid_extension(filename))
                        paths.push_back(filename);
                }

                else if (boost::filesystem::is_directory(filename))
                {
                    // iterate over each file in the directory
                    for (auto& file : boost::make_iterator_range(boost::filesystem::directory_iterator(filename), {}))
                    {
                        if (boost::filesystem::is_regular_file(file))
                        {
                            // collect valid files only
                            if (is_valid_extension(file))
                                paths.push_back(file.path().string());
                        }
                    }
                }
                else // existing file but a weird one
                    env->out() << "[e] given file name does not point to a regular file" << std::endl;
            }
            else // path leads nowhere
                env->out() << "[e] given file name does not exist" << std::endl;

            // sort by file size to make the small ones go first
            if (sort)
            {
                std::sort(paths.begin(), paths.end(), [](std::string& f1, std::string& f2)
                {
                    return boost::filesystem::file_size(f1) >= boost::filesystem::file_size(f2);
                });
            }

            // handle collected files
            for (const auto& f : paths)
            {
                // parse Verilog
                if (boost::filesystem::extension(f) == ".v")
                {
                    read<mockturtle::verilog_reader<logic_network::mig_nt>,
                            lorina::return_code(const std::string&,
                                                const lorina::verilog_reader&,
                                                lorina::diagnostic_engine*)>(f, lorina::read_verilog);
                }
                // parse Aiger
                else if (boost::filesystem::extension(f) == ".aig")
                {
                    read<mockturtle::aiger_reader<logic_network::mig_nt>,
                            lorina::return_code(const std::string&,
                                                const lorina::aiger_reader&,
                                                lorina::diagnostic_engine*)>(f, lorina::read_aiger);
                }
                // parse ...
                // else if (boost::filesystem::extension(f) == ...)
            }

            // reset flags, necessary for some reason... alice bug?
            paths = {};
            sort = false;
        }

    private:
        /**
         * Verilog filename.
         */
        std::string filename;
        /**
         * All paths found under filename.
         */
        std::vector<std::string> paths{};
        /**
         * Flag to indicate that files should be sorted by file size.
         */
        bool sort = false;
        /**
         * Actual read function that constructs the logic network from a file.
         *
         * @tparam Reader The functor type which constructs the network, i.e. handles the callbacks.
         * @tparam ReadFun The file parser type.
         * @param file The file name.
         * @param rfun The actual parsing function.
         */
        template <class Reader, class ReadFun>
        void read(const std::string& file, ReadFun rfun) const noexcept
        {
            auto name = boost::filesystem::path{file}.stem().string();
            logic_network::mig_nt mig;

            try
            {
                if (lorina::diagnostic_engine diag{};
                        rfun(file, Reader{mig}, &diag) == lorina::return_code::success)
                {
                    if (mig.is_combinational())
                        store<logic_network_ptr>().extend() = std::make_shared<logic_network>(std::move(mig), std::move(name));
                    else
                        env->out() << "[e] sequential networks are not yet supported for FCN physical design" << std::endl;
                }
                else
                {
                    env->out() << "[e] parsing error in " << file << std::endl;
                }
            }
            catch (...)
            {
                env->out() << "[e] " << file << " contains unsupported features" << std::endl;
            }
        }
    };

    ALICE_ADD_COMMAND(read, "I/O")
}


#endif //FICTION_READ_H
