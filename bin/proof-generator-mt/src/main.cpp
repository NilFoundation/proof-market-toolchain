//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2022-2023 Aleksei Moskvin <alalmoskvin@nil.foundation>
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

#include <nil/actor/core/app_template.hh>
#include <nil/actor/core/reactor.hh>
#include <nil/actor/core/metrics_api.hh>
#include <nil/actor/core/print.hh>

#include <nil/actor/core/thread.hh>
#include <nil/actor/core/with_scheduling_group.hh>

#include <nil/proof-generator-mt/aspects/actor.hpp>
#include <nil/proof-generator-mt/aspects/args.hpp>
#include <nil/proof-generator-mt/aspects/path.hpp>
#include <nil/proof-generator-mt/aspects/configuration.hpp>
#include <nil/proof-generator-mt/aspects/prover_vanilla.hpp>
#include <nil/proof-generator-mt/detail/configurable.hpp>
#include <nil/proof-generator-mt/circuits/mina-state/proof.hpp>
#include <nil/proof-generator-mt/assigner/proof.hpp>

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
::
    boost::shared_ptr<nil::proof_generator_mt::aspects::path> path_aspect = boost::make_shared<nil::proof_generator_mt::aspects::path>();

    ctx.insert<nil::proof_generator_mt::aspects::path>(path_aspect);
    ctx.insert<nil::proof_generator_mt::aspects::configuration>(boost::make_shared<nil::proof_generator_mt::aspects::configuration>(path_aspect));
    ctx.insert<nil::proof_generator_mt::aspects::actor>(boost::make_shared<nil::proof_generator_mt::aspects::actor>(path_aspect));
    ctx.insert<nil::proof_generator_mt::aspects::prover_vanilla>(boost::make_shared<nil::proof_generator_mt::aspects::prover_vanilla>(path_aspect));

    return true;
}

template<typename Application>
inline bool configure_aspects(boost::application::context &ctx, Application &app) {
    typedef nil::proof_generator_mt::detail::configurable<nil::dbms::plugin::variables_map,
                                             nil::dbms::plugin::cli_options_description,
                                             nil::dbms::plugin::cfg_options_description>
        configurable_aspect_type;

    boost::strict_lock<boost::application::aspect_map> guard(ctx);
    boost::shared_ptr<nil::proof_generator_mt::aspects::args> args = ctx.find<nil::proof_generator_mt::aspects::args>(guard);
    boost::shared_ptr<nil::proof_generator_mt::aspects::configuration> cfg = ctx.find<nil::proof_generator_mt::aspects::configuration>(guard);

    for (boost::shared_ptr<void> itr : ctx) {
        boost::static_pointer_cast<configurable_aspect_type>(itr)->set_options(cfg->cli());
        boost::static_pointer_cast<configurable_aspect_type>(itr)->set_options(cfg->cfg());
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

nil::actor::future<> proof_new(boost::json::value circuit_description, boost::json::value public_input, std::string output_file, std::string example = "") {
    return nil::actor::async([&] {
        std::string statement_type = boost::json::value_to<std::string>(circuit_description.at("type"));
        if (statement_type == "zkllvm") {
            std::string bytecode = boost::json::value_to<std::string>(circuit_description.at("statement"));
            std::string public_input_str = boost::json::value_to<std::string>(public_input.at("input"));
            nil::proof_generator_mt::assigner::proof_new(bytecode, public_input_str, output_file);
        } else {
            boost::json::value statement = circuit_description.at("statement");
            boost::json::value public_input_mina = public_input.at("input");
            nil::proof_generator_mt::mina_state::proof_new(statement, public_input_mina, output_file);
        }
    });
}

struct prover {
    prover(boost::application::context &context) : context_(context) {
    }

    int operator()() {
        BOOST_APPLICATION_FEATURE_SELECT
        std::string json_circuit = context_.find<nil::proof_generator_mt::aspects::prover_vanilla>()->input_circuit_string();
        std::string json_public_input = context_.find<nil::proof_generator_mt::aspects::prover_vanilla>()->input_public_params_string();
        std::string output_file = context_.find<nil::proof_generator_mt::aspects::prover_vanilla>()->output_file_string();
        boost::json::error_code ec;

        boost::json::value circuit_description = boost::json::parse( json_circuit, ec );
        if( ec )
            std::cout << "Json circuit description parsing failed: " << ec.message() << "\n";
        boost::json::value public_input = boost::json::parse( json_public_input, ec );
        if( ec )
            std::cout << "Json public_input parsing failed: " << ec.message() << "\n";

        (void)nil::actor::engine().when_started().then(
                [circuit_description, public_input, output_file, json_circuit]() {return proof_new(circuit_description, public_input, output_file, json_circuit);}).then_wrapped([](auto &&f) {
            try {
                nil::actor::engine().exit(0);
            } catch (std::exception &ex) {
                nil::actor::engine().exit(1);
            }
        });
        auto exit_code = nil::actor::engine().run();
        std::cout << exit_code << std::endl;
        nil::actor::smp::cleanup();

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

    if (!insert_aspects(ctx, app, boost::make_shared<nil::proof_generator_mt::aspects::args>(argc, argv))) {
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