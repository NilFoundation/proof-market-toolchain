//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022-2023 Ilia Shirobokov <i.shirobokov@nil.foundation>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//---------------------------------------------------------------------------//

#ifndef PROOF_GENERATOR_ASSIGNER_PROOF_HPP
#define PROOF_GENERATOR_ASSIGNER_PROOF_HPP

#include <sstream>
#include <fstream>

#include <nil/crypto3/algebra/curves/pallas.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/blueprint/parser.hpp>
#include <nil/blueprint/utils/table_profiling.hpp>
#include <nil/blueprint/utils/satisfiability_check.hpp>

#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/field_type.hpp>
#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/plonk/constraint_system.hpp>
#include <nil/crypto3/multiprecision/cpp_int.hpp>

namespace nil {
    namespace proof_generator {
        namespace assigner {

            template<typename TIter>
            void print_hex_byteblob(std::ostream &os, TIter iter_begin, TIter iter_end, bool endl) {
                os << "0x" << std::hex;
                for (TIter it = iter_begin; it != iter_end; it++) {
                    os << std::setfill('0') << std::setw(2) << std::right << int(*it);
                }
                os << std::dec;
                if (endl) {
                    os << std::endl;
                }
            }

            template<typename Endianness, typename ConstraintSystemType>
            void print_circuit(const ConstraintSystemType &circuit, std::ostream &out = std::cout) {

                using TTypeBase = nil::marshalling::field_type<Endianness>;
                auto filled_val =
                    nil::crypto3::marshalling::types::fill_plonk_constraint_system<ConstraintSystemType, Endianness>(
                        circuit);

                std::vector<std::uint8_t> cv;
                cv.resize(filled_val.length(), 0x00);
                auto write_iter = cv.begin();
                nil::marshalling::status_type status = filled_val.write(write_iter, cv.size());
                print_hex_byteblob(out, cv.cbegin(), cv.cend(), false);
            }

            void proof_new(std::string bytecode, std::string public_input_str, std::string output_file) {
                using curve_type = nil::crypto3::algebra::curves::pallas;
                using BlueprintFieldType = typename curve_type::base_field_type;
                constexpr std::size_t WitnessColumns = 15;
                constexpr std::size_t PublicInputColumns = 5;
                constexpr std::size_t ConstantColumns = 5;
                constexpr std::size_t SelectorColumns = 50;

                using ArithmetizationParams =
                    nil::crypto3::zk::snark::plonk_arithmetization_params<WitnessColumns, PublicInputColumns,
                                                                          ConstantColumns, SelectorColumns>;
                using ConstraintSystemType =
                    nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>;

                std::vector<typename BlueprintFieldType::value_type> public_input;
                nil::crypto3::multiprecision::cpp_int number;
                std::stringstream ss;
                ss << public_input_str;

                while (!ss.eof()) {
                    ss >> number;
                    public_input.push_back(number);
                }
                nil::blueprint::parser<BlueprintFieldType, ArithmetizationParams> parser_instance;

                std::string bytecode_file_name = output_file + "_tmp.ll";
                std::ofstream out(bytecode_file_name);
                out << bytecode;
                out.close();

                std::unique_ptr<llvm::Module> module = parser_instance.parseIRFile(bytecode_file_name.c_str());
                if (module == nullptr) {
                    return;
                }

                if (!parser_instance.evaluate(*module, public_input)) {
                    return;
                }

                nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType, ArithmetizationParams> desc;
                desc.usable_rows_amount = parser_instance.assignmnt.rows_amount();
                desc.rows_amount = nil::crypto3::zk::snark::basic_padding(parser_instance.assignmnt);

                std::cout << "generatring zkllvm proof..." << std::endl;

                std::ofstream otable;
                std::string assignment_table_file_name = output_file + ".table";
                otable.open(assignment_table_file_name);
                if (!otable) {
                    std::cout << "Something wrong with output " << assignment_table_file_name << std::endl;
                    return;
                }
                nil::blueprint::profiling_assignment_table(parser_instance.assignmnt, desc.usable_rows_amount, otable);
                otable.close();

                std::ofstream ocircuit;
                std::string circuit_file_name = output_file;
                ocircuit.open(circuit_file_name);
                if (!ocircuit) {
                    std::cout << "Something wrong with output " << circuit_file_name << std::endl;
                    return;
                }
                print_circuit<nil::marshalling::option::big_endian, ConstraintSystemType>(parser_instance.bp, ocircuit);
                ocircuit.close();

                std::cout << "zkllvm proof is generated" << std::endl;

                if (nil::blueprint::is_satisfied(parser_instance.bp, parser_instance.assignmnt)) {
                    std::cout << "Inner verification passed" << std::endl;
                } else {
                    std::cout << "Inner verification failed" << std::endl;
                }

                std::remove(bytecode_file_name.c_str());
            }
        }    // namespace assigner
    }        // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_ASSIGNER_PROOF_HPP