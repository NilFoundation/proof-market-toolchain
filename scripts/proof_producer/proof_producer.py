"""Proof Producer daemon PoC"""
import argparse
import json
import logging
import os
import selectors
import subprocess
import sys
import tempfile
import time
import traceback
from threading import Lock, Thread
from urllib.parse import urljoin

from retrying import retry

# TODO: restructure project
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/../")
import constants
from auth_tools import Authenticator, get_headers
from proof_tools import push as push_proof
from proposal_tools import get as get_proposals
from public_input_get import get as get_public_input
from statement_tools import get_statements


logger = logging.getLogger(__name__)


class CommandRunner():
    @staticmethod
    def _log_stream(selector, logger_method, prefix):
        while True:
            for key, _ in selector.select():
                data = key.fileobj.readline()
                if not data:
                    return
                logger_method(f"{prefix}: {data.strip()}")

    @staticmethod
    def run(command, prefix="Subprocess") -> int:
        process = subprocess.Popen(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1,
        )

        sel = selectors.DefaultSelector()
        sel.register(process.stdout, selectors.EVENT_READ)
        sel.register(process.stderr, selectors.EVENT_READ)

        CommandRunner._log_stream(sel, logger.info, prefix)
        CommandRunner._log_stream(sel, logger.error, prefix)

        process.wait()

        return process.returncode


def exponential_none_retry(func):
    return retry(
        retry_on_result=lambda x: x is None,
        stop_max_attempt_number=1,
        wait_exponential_multiplier=1 * 1000,
        wait_exponential_max=20 * 1000,
    )(func)


def periodic(interval):
    def decorator(func):
        def wrapper(*args, **kwargs):
            while True:
                try:
                    func(*args, **kwargs)
                except Exception as e:
                    logger.error(f"An error occurred: {e}")
                    logger.error(traceback.format_exc())
                time.sleep(interval)
        return wrapper
    return decorator


class ProofProducerDaemon:
    TASK_FETCH_PERIOD = 30
    AUTH_PERIOD = 20 * 60

    def __init__(self, base_url, db_path, statements_file, proof_generator_binary_path, assigner_binary_path, log_level, limit):
        self.base_url = base_url
        self.db_path = db_path
        self.proof_generator_binary_path = proof_generator_binary_path
        self.assigner_binary_path = assigner_binary_path
        self.log_level = log_level
        self.limit = limit
        self.authenticator = Authenticator(base_url)
        self.processing_tasks = set()

        self.statements_ids_to_process = None
        if statements_file is not None:
            # If file is provided, we process only ids from it. Otherwise, we process everything.
            with open(statements_file, 'r') as f:
                self.statements_ids_to_process = [l.strip() for l in f if l.strip()]

    def fetch_statements(self):
        logger.info("Fetching statements from proof market...")
        statements_list = get_statements(self.base_url)
        if self.statements_ids_to_process is not None:
            statements_list = list(filter(lambda s: str(s["id"]) in self.statements_ids_to_process, statements_list))

        statements = {}
        error_met = False
        for key in statements_list:
            try:
                statements[key["id"]] = get_statements(self.base_url, str(key["id"]), None)
            except Exception as e:
                error_met = True
                logger.error("Get statement error: ", e)
                continue
        none_statements_n = sum(map(lambda x: x is None, statements.values()))
        logger.info(f"{len(statements) - none_statements_n} statements were loaded, {none_statements_n} were not.")

        statements_dir = os.path.join(self.db_path, "statements")
        os.makedirs(statements_dir, exist_ok=True)
        for key in statements:
            with open(os.path.join(statements_dir, f"{str(key)}.json"), "w") as f:
                json.dump(statements[key], f, indent=2)
        logging.info("Statements saved to data base.")
        return not error_met

    def process_single_task(self, request_key: str, statement_key: str):
        try:
            logger.info(f"Generating proof for {request_key}...")
            res = self.process_assigned_proposal(
                request_key,
                statement_key,
            )
            logger.info(f"Generating proof for {request_key} was {'successful' if res else 'unsuccessful'}")
        finally:
            self.processing_tasks.remove(request_key)

    def run_assigned_tasks(self):
        # For now /proposal/ returns only assigned to current producer, undone requests.
        # No need to ask for assignment.
        logger.info("Fetching proposals...")
        proposals = exponential_none_retry(get_proposals)(self.base_url)
        if proposals is None:
            logger.error("Can't get proposals list.")
            return False

        if self.statements_ids_to_process is not None:
            proposals_for_current_worker = list(filter(lambda p: p["statement_key"] in self.statements_ids_to_process, proposals))
        else:
            proposals_for_current_worker = proposals

        assigned_amount = len(proposals_for_current_worker)
        can_process = self.limit - len(self.processing_tasks)
        logger.info(
            "%d are assigned to current worker. Remaining slots for processing: %d",
            assigned_amount,
            can_process,
        )
        if can_process == 0:
            return True

        unprocessed_proposals = list(filter(lambda p: p["request_key"] not in self.processing_tasks, proposals_for_current_worker))
        for proposal in unprocessed_proposals[:can_process]:
            self.processing_tasks.add(proposal["request_key"])
            Thread(target=self.process_single_task, args=(proposal["request_key"], proposal["statement_key"])).start()

    def process_assigned_proposal(self, request_key, statement_key):
        try:
            with open(os.path.join(self.db_path, "statements", str(statement_key) + ".json"), 'r') as file:
                data = json.load(file)
            bytecode_data = data['definition']['proving_key']
        except Exception as e:
            logger.error(
                "Bytecode parsing error from statement file %s\nError: %s",
                self.statement_path,
                e,
            )
            return False

        public_input_data = exponential_none_retry(get_public_input)(request_key, self.base_url)
        if public_input_data is None:
            logger.error("Can't get public input, no further attempts will be made.")
            return False

        with tempfile.NamedTemporaryFile(mode='w', delete=False) as public_input_fd, \
             tempfile.NamedTemporaryFile(mode='w', delete=False) as bytecode_fd, \
             tempfile.NamedTemporaryFile(mode='w', delete=False) as assignment_table_fd, \
             tempfile.NamedTemporaryFile(mode='w', delete=False) as circuit_fd, \
             tempfile.NamedTemporaryFile(mode='w', delete=False) as proof_fd:

            json.dump(public_input_data, public_input_fd, indent=2)
            public_input_fd.flush()

            bytecode_fd.write(bytecode_data)
            bytecode_fd.flush()

            logger.info("Running assigner")
            assigner_ret = CommandRunner.run(
                [
                    self.assigner_binary_path,
                    "--bytecode=" + bytecode_fd.name,
                    "--public-input=" + public_input_fd.name,
                    "--circuit=" + circuit_fd.name,
                    "--assignment-table=" + assignment_table_fd.name,
                    "--elliptic-curve-type=" + "pallas",
                    "--log-level=" + self.log_level,
                ],
                f'{request_key} assigner'
            )
            if assigner_ret != 0:
                logger.error("Assigner failed.")
                return False

            logger.info("Running proof generator")
            proof_generator_ret = CommandRunner.run(
                [
                    self.proof_generator_binary_path,
                    "--circuit=" + circuit_fd.name,
                    "--assignment-table=" + assignment_table_fd.name,
                    "--proof=" + proof_fd.name,
                    "--log-level=" + self.log_level,
                ],
                f'{request_key} generator'
            )
            if proof_generator_ret != 0:
                logger.error("Proof generator failed.")
                return False

            push_result = exponential_none_retry(push_proof)(
                self.base_url,
                proof_fd.name,
                request_key=request_key,
                proposal_key=request_key,
            )
            if push_result is None:
                logger.error("Can't push proof, no further attempts will be made.")
                return False

            return True

    def start(self):
        self.authenticator.update_auth_file()
        threads = [
            Thread(
                daemon=True,
                target=periodic(interval=self.AUTH_PERIOD)(
                    self.authenticator.update_auth_file
                )
            ),
            Thread(
                daemon=True,
                target=periodic(interval=self.TASK_FETCH_PERIOD)(
                    self.run_assigned_tasks
                )
            ),
        ]
        for thread in threads:
            thread.start()

        for thread in threads:
            # Will never reach it, they are immortal
            thread.join()


def prepare(args):
    daemon = ProofProducerDaemon(
        base_url=args.url,
        db_path=args.db_path,
        statements_file=args.statements,
        proof_generator_binary_path=None,
        assigner_binary_path=None,
        log_level=None,
        limit=None,
    )
    return daemon.fetch_statements()


def start(args):
    daemon = ProofProducerDaemon(
        base_url=args.url,
        db_path=args.db_path,
        statements_file=args.statements,
        proof_generator_binary_path=args.proof_generator,
        assigner_binary_path=args.assigner,
        log_level=args.log_level,
        limit=args.limit,
    )
    daemon.start()


def main():
    logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")

    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument("--url", action="store", default=constants.URL, help="URL of a producer")
    parent_parser.add_argument(
        "--db-path", help="directory with producer's data", default="/tmp/proof-producer/"
    )
    parent_parser.add_argument("--statements", action="store", required=True, help="Files with statements IDs you want to process. One ID per line.")

    parser = argparse.ArgumentParser()
    # If we ever need more sub-commands
    subparsers = parser.add_subparsers(help="sub-command help")

    parser_prepare = subparsers.add_parser(
        "prepare",
        help="Fetch statements from proof market. Only statements provided in --statements file will be fetched.",
        parents=[parent_parser],
    )
    parser_prepare.set_defaults(func=prepare)

    parser_start = subparsers.add_parser(
        "start",
        help="start Proof Producer daemon (do not forget to call `prepare` first)",
        parents=[parent_parser],
    )
    parser_start.add_argument(
        "--proof-generator", help="path to proof generator binary", required=True
    )
    parser_start.add_argument(
        "--assigner", help="path to assigner binary", required=True
    )
    parser_start.add_argument(
        "--log-level", help="log level", choices=['trace', 'debug', 'info', 'warning', 'error', 'fatal'], default="info"
    )
    parser_start.add_argument(
        "--limit", help="Number of tasks we want to process simultaneously", default=1
    )
    parser_start.set_defaults(func=start)

    args = parser.parse_args()
    if not hasattr(args, 'func'):
        # invalid subparser
        parser.print_help()
        sys.exit(1)

    sys.exit(0 if args.func(args) else 1)


if __name__ == "__main__":
    main()
