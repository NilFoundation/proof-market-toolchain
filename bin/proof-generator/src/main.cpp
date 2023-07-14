//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022 Aleksei Moskvin <alalmoskvin@nil.foundation>
// Copyright (c) 2022 Ilia Shirobokov <i.shirobokov@nil.foundation>
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

#include <iostream>
#include <chrono>

#define BOOST_APPLICATION_FEATURE_NS_SELECT_BOOST

#include <boost/application.hpp>

#undef B0

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/json/src.hpp>
#include <boost/optional.hpp>
#include <boost/program_options/parsers.hpp>

#include <nil/proof-generator/aspects/args.hpp>
#include <nil/proof-generator/aspects/path.hpp>
#include <nil/proof-generator/aspects/configuration.hpp>
#include <nil/proof-generator/aspects/prover_vanilla.hpp>
#include <nil/proof-generator/detail/configurable.hpp>

#include <nil/proof-generator/circuits/mina-state/proof.hpp>
#include <nil/proof-generator/assigner/proof.hpp>

template<typename F, typename First, typename... Rest>
inline void insert_aspect(F f, First first, Rest... rest) {
    f(first);
    insert_aspect(f, rest...);
}

template<typename F>
inline void insert_aspect(F f) {
}

template<typename Application, typename... Aspects>
inline bool insert_aspects(boost::application::context &ctx, Application &app, Aspects... args) {
    insert_aspect([&](auto aspect) { ctx.insert<typename decltype(aspect)::element_type>(aspect); }, args...);
    ::boost::shared_ptr<nil::proof_generator::aspects::path> path_aspect =
        boost::make_shared<nil::proof_generator::aspects::path>();

    ctx.insert<nil::proof_generator::aspects::path>(path_aspect);
    ctx.insert<nil::proof_generator::aspects::configuration>(
        boost::make_shared<nil::proof_generator::aspects::configuration>(path_aspect));
    ctx.insert<nil::proof_generator::aspects::prover_vanilla>(
        boost::make_shared<nil::proof_generator::aspects::prover_vanilla>(path_aspect));

    return true;
}

template<typename Application>
inline bool configure_aspects(boost::application::context &ctx, Application &app) {
    typedef nil::proof_generator::detail::configurable<nil::dbms::plugin::variables_map,
                                                       nil::dbms::plugin::cli_options_description,
                                                       nil::dbms::plugin::cfg_options_description>
        configurable_aspect_type;

    boost::strict_lock<boost::application::aspect_map> guard(ctx);
    boost::shared_ptr<nil::proof_generator::aspects::args> args = ctx.find<nil::proof_generator::aspects::args>(guard);
    boost::shared_ptr<nil::proof_generator::aspects::configuration> cfg =
        ctx.find<nil::proof_generator::aspects::configuration>(guard);

    for (boost::shared_ptr<void> itr : ctx) {
        boost::static_pointer_cast<configurable_aspect_type>(itr)->set_options(cfg->cli());
        // boost::static_pointer_cast<configurable_aspect_type>(itr)->set_options(cfg->cfg());
    }

    try {
        boost::program_options::store(
            boost::program_options::parse_command_line(args->argc(), args->argv(), cfg->cli()), cfg->vm());
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    for (boost::shared_ptr<void> itr : ctx) {
        boost::static_pointer_cast<configurable_aspect_type>(itr)->initialize(cfg->vm());
    }

    return false;
}

void proof_new(boost::json::value circuit_description, boost::json::value public_input, std::string output_file) {
    std::string statement_type = boost::json::value_to<std::string>(circuit_description.at("type"));
    if (statement_type == "placeholder-zkllvm") {
        std::string bytecode =
            boost::json::value_to<std::string>(circuit_description.at("definition").at("proving_key"));
        nil::proof_generator::assigner::proof_new(bytecode, public_input, output_file);
    } else if (statement_type == "placeholder-vanilla") {
        boost::json::value statement = circuit_description.at("definition").at("proving_key");
        nil::proof_generator::mina_state::proof_new(statement, public_input, output_file);
    } else {
        std::cout << "Unknown statement type: " << statement_type << "\n";
    }
}

struct prover {
    prover(boost::application::context &context) : context_(context) {
    }

    int operator()() {
        BOOST_APPLICATION_FEATURE_SELECT
        std::string json_circuit =
            context_.find<nil::proof_generator::aspects::prover_vanilla>()->input_circuit_string();
        std::string json_public_input =
            context_.find<nil::proof_generator::aspects::prover_vanilla>()->input_public_params_string();
        std::string output_file = context_.find<nil::proof_generator::aspects::prover_vanilla>()->output_file_string();
        boost::json::error_code ec;

        boost::json::value circuit_description = boost::json::parse(json_circuit, ec);
        if (ec)
            std::cout << "Json circuit description parsing failed: " << ec.message() << "\n";
        boost::json::value public_input = boost::json::parse(json_public_input, ec);
        if (ec)
            std::cout << "Json public_input parsing failed: " << ec.message() << "\n";

        proof_new(circuit_description, public_input, output_file);

        return 0;
    }

    boost::application::context &context_;
};

bool setup(boost::application::context &context) {
    return false;
}

int main(int argc, char *argv[]) {
    boost::system::error_code ec;
    /*<<Create a global context application aspect pool>>*/
    boost::application::context ctx;

    boost::application::auto_handler<prover> app(ctx);

    if (!insert_aspects(ctx, app, boost::make_shared<nil::proof_generator::aspects::args>(argc, argv))) {
        std::cout << "[E] Application aspects configuration failed!" << std::endl;
        return 1;
    }
    if (configure_aspects(ctx, app)) {
        std::cout << "[I] Setup changed the current configuration." << std::endl;
    }
    // my server instantiation
    int result = boost::application::launch<boost::application::common>(app, ctx, ec);

    if (ec) {
        std::cout << "[E] " << ec.message() << " <" << ec.value() << "> " << std::endl;
    }

    return result;
}