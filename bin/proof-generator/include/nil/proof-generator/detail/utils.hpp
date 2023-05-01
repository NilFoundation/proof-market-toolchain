//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
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

#ifndef PROOF_GENERATOR_DETAIL_UTILS_HPP
#define PROOF_GENERATOR_DETAIL_UTILS_HPP

#include <nil/marshalling/endianness.hpp>
#include <nil/crypto3/marshalling/zk/types/placeholder/proof.hpp>
#include <nil/marshalling/status_type.hpp>

#include <fstream>

namespace nil {
    namespace proof_generator {
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

        template<typename Endianness, typename Proof>
        void proof_print(const Proof &proof, std::string output_file) {
            using namespace nil::crypto3::marshalling;

            using TTypeBase = nil::marshalling::field_type<Endianness>;
            using proof_marshalling_type = nil::crypto3::zk::snark::placeholder_proof<TTypeBase, Proof>;
            auto filled_placeholder_proof =
                crypto3::marshalling::types::fill_placeholder_proof<Endianness, Proof>(proof);

            std::vector<std::uint8_t> cv;
            cv.resize(filled_placeholder_proof.length(), 0x00);
            auto write_iter = cv.begin();
            nil::marshalling::status_type status = filled_placeholder_proof.write(write_iter, cv.size());
            std::ofstream out;
            out.open(output_file);
            print_hex_byteblob(out, cv.cbegin(), cv.cend(), false);
        }
    }    // namespace proof_generator
}    // namespace nil

#endif    // PROOF_GENERATOR_DETAIL_UTILS_HPP