//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the Server Side Public License, version 1,
// as published by the author.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Server Side Public License for more details.
//
// You should have received a copy of the Server Side Public License
// along with this program. If not, see
// <https://github.com/NilFoundation/dbms/blob/master/LICENSE_1_0.txt>.
//---------------------------------------------------------------------------//

#include <nil/proof-generator/aspects/prover_vanilla.hpp>

#include <boost/exception/get_error_info.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/string_file.hpp>

namespace std {
    template<typename CharT, typename TraitsT>
    std::basic_ostream<CharT, TraitsT> &operator<<(std::basic_ostream<CharT, TraitsT> &out,
                                                   const std::vector<std::string> &xs) {
        return out << std::accumulate(
                   std::next(xs.begin()), xs.end(), xs[0],
                   [](std::string a, const std::string &b) -> std::string { return std::move(a) + ';' + b; });
    }
}    // namespace std

namespace nil {
    namespace proof_generator {
        namespace aspects {
            prover_vanilla::prover_vanilla(boost::shared_ptr<path> aspct) : path_aspect(std::move(aspct)) {
            }

            void prover_vanilla::set_options(cli_options_type &cli) const {
                boost::program_options::options_description options("NIL Proof Generator");
                // clang-format off
                options.add_options()
                ("version,v", "Display version")
                ("proof_out", boost::program_options::value<std::string>(),"Output file")
                ("circuit_input", boost::program_options::value<std::string>(), "Circuit input file")
                ("public_input", boost::program_options::value<std::string>(), "Public params input file");
                // clang-format on
                cli.add(options);
            }

            void prover_vanilla::set_options(cfg_options_type &cfg) const {
                boost::program_options::options_description options("NIL Proof Generator");
                // clang-format off
                options.add_options()
                ("version,v", "Display version")
                ("proof_out", boost::program_options::value<std::string>(),"Output file")
                ("circuit_input", boost::program_options::value<std::string>(), "Circuit input file")
                ("public_input", boost::program_options::value<std::string>(), "Public params input file");
                // clang-format on
                cfg.add(options);
            }

            void prover_vanilla::initialize(configuration_type &vm) {
                if (vm.count("circuit_input")) {
                    if (vm["circuit_input"].as<std::string>().size() < PATH_MAX ||
                        vm["circuit_input"].as<std::string>().size() < FILENAME_MAX) {
                        if (boost::filesystem::exists(vm["circuit_input"].as<std::string>())) {
                            boost::filesystem::load_string_file(vm["circuit_input"].as<std::string>(), json_circuit);
                        }
                    } else {
                        json_circuit = vm["circuit_input"].as<std::string>();
                    }
                }
                if (vm.count("public_input")) {
                    if (vm["public_input"].as<std::string>().size() < PATH_MAX ||
                        vm["public_input"].as<std::string>().size() < FILENAME_MAX) {
                        if (boost::filesystem::exists(vm["public_input"].as<std::string>())) {
                            boost::filesystem::load_string_file(vm["public_input"].as<std::string>(),
                                                                json_public_input);
                        }
                    } else {
                        json_public_input = vm["public_input"].as<std::string>();
                    }
                }
                if (vm.count("proof_out")) {
                    output_file = vm["proof_out"].as<std::string>();
                }
            }

            boost::filesystem::path prover_vanilla::default_config_path() const {
                return path_aspect->config_path() / "config.ini";
            }

            std::string prover_vanilla::input_circuit_string() const {
                return json_circuit;
            }

            std::string prover_vanilla::input_public_params_string() const {
                return json_public_input;
            }

            std::string prover_vanilla::output_file_string() const {
                return output_file;
            }
        }    // namespace aspects
    }        // namespace proof_generator
}    // namespace nil
